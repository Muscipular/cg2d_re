#include "LegacyMidiSynth.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

namespace cg::platform {
namespace {

using Bytes = std::span<const std::uint8_t>;
std::uint32_t ReadLe(Bytes bytes, std::size_t at, unsigned width)
{
    std::uint32_t value = 0;
    for (unsigned i = 0; i < width; ++i) { value |= std::uint32_t(bytes[at + i]) << (8 * i); }
    return value;
}
std::uint32_t ReadBe(Bytes bytes, std::size_t at, unsigned width)
{
    std::uint32_t value = 0;
    for (unsigned i = 0; i < width; ++i) { value = (value << 8) | bytes[at + i]; }
    return value;
}
bool Tag(Bytes bytes, std::size_t at, const char* text)
{
    return at <= bytes.size() && bytes.size() - at >= 4 && std::memcmp(bytes.data() + at, text, 4) == 0;
}
struct Chunk { std::size_t header; Bytes data; };
// RIFF块只借文件内存，解析完成后由样本/区域独立持有所需数据。
bool Chunks(Bytes bytes, std::vector<Chunk>& output)
{
    for (std::size_t at = 0; at < bytes.size();) {
        if (bytes.size() - at < 8) { return false; }
        const auto length = ReadLe(bytes, at + 4, 4);
        if (length > bytes.size() - at - 8) { return false; }
        output.push_back({at, bytes.subspan(at + 8, length)});
        const std::size_t next = at + 8 + length + (length & 1u);
        if (next > bytes.size()) { return false; }
        at = next;
    }
    return true;
}
const Chunk* Find(Bytes parent, const std::vector<Chunk>& chunks, const char* name, const char* list = nullptr)
{
    for (const auto& chunk : chunks) {
        if (Tag(parent, chunk.header, name) && (list == nullptr || Tag(chunk.data, 0, list))) { return &chunk; }
    }
    return nullptr;
}
struct SampleParameters {
    std::uint16_t unity {60};
    std::int16_t fine {};
    std::int32_t attenuation {};
    std::uint32_t loopStart {};
    std::uint32_t loopLength {};
};
bool ReadSampleParameters(Bytes bytes, SampleParameters& out)
{
    if (bytes.size() < 20) { return false; }
    const auto size = ReadLe(bytes, 0, 4);
    const auto loops = ReadLe(bytes, 16, 4);
    if (size < 20 || size > bytes.size()) { return false; }
    out.unity = static_cast<std::uint16_t>(ReadLe(bytes, 4, 2));
    out.fine = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(ReadLe(bytes, 6, 2)));
    out.attenuation = std::bit_cast<std::int32_t>(ReadLe(bytes, 8, 4));
    if (loops != 0) {
        if (bytes.size() - size < 16 || ReadLe(bytes, size, 4) < 16) { return false; }
        out.loopStart = ReadLe(bytes, size + 8, 4);
        out.loopLength = ReadLe(bytes, size + 12, 4);
    }
    return true;
}
struct Articulation {
    double attack {};
    double decay {};
    double release {};
    double sustain {1.0};
    double pan {};
};
bool ReadArticulation(Bytes parent, const std::vector<Chunk>& chunks, Articulation& out)
{
    const Chunk* list = Find(parent, chunks, "LIST", "lart");
    if (list == nullptr) { return true; }
    std::vector<Chunk> items;
    const Bytes body = list->data.subspan(4);
    if (!Chunks(body, items)) { return false; }
    const Chunk* art = Find(body, items, "art1");
    if (art == nullptr) { return true; }
    if (art->data.size() < 8) { return false; }
    const auto size = ReadLe(art->data, 0, 4), count = ReadLe(art->data, 4, 4);
    if (size < 8 || size > art->data.size() || count > (art->data.size() - size) / 12) { return false; }
    for (std::size_t i = 0; i < count; ++i) {
        const auto at = size + 12 * i;
        // 常量连接；动态控制消息另由MIDI通道持有，不猜成常量连接。
        if (ReadLe(art->data, at, 2) != 0 || ReadLe(art->data, at + 2, 2) != 0) { continue; }
        const auto destination = ReadLe(art->data, at + 4, 2);
        const auto raw = std::bit_cast<std::int32_t>(ReadLe(art->data, at + 8, 4));
        const double scale = static_cast<double>(raw) / 65536.0;
        const double seconds = raw == (std::numeric_limits<std::int32_t>::min)() ? 0.0 : std::exp2(scale / 1200.0);
        switch (destination) {
        case 0x206: out.attack = seconds; break;
        case 0x207: out.decay = seconds; break;
        case 0x209: out.release = seconds; break;
        case 0x20A: out.sustain = scale / 1000.0; break;
        case 4: out.pan = scale / 500.0; break;
        default: break;
        }
    }
    return true;
}

} // namespace

struct LegacyMidiSynth::Impl {
    struct Wave { std::vector<float> stereo; std::uint32_t frequency; SampleParameters parameters; std::size_t poolOffset; };
    struct Region {
        std::uint32_t bank, program, wave;
        std::uint16_t keyLow, keyHigh, velocityLow, velocityHigh;
        SampleParameters parameters;
        bool ownParameters {};
        Articulation articulation;
    };
    struct Event { std::uint64_t tick, frame; std::uint8_t status, first, second; std::uint32_t tempo; };
    struct Channel {
        std::array<std::uint8_t, 128> control {};
        std::uint8_t program {};
        std::uint16_t bend {8192};
        // RPN0/1/2各自持有参数；切换选择不丢值，序列重置由channels={}恢复。
        std::uint8_t bendSemitones {2}, bendCents {};
        std::uint16_t fineTuning {8192};
        std::uint8_t coarseTuning {64};
        bool nrpnSelected {}; // CC99/98后Data Entry不再写RPN，CC101/100重新选择RPN。
    };
    struct Voice {
        const Region* region {};
        const Wave* wave {};
        SampleParameters parameters;
        std::uint8_t channel {}, key {}, velocity {};
        double cursor {}, envelope {}, releasedLevel {};
        std::uint64_t age {}, releaseAge {};
        bool released {}, keyUp {};
    };
    std::vector<Wave> waves;
    std::vector<Region> regions;
    std::vector<Event> events;
    std::array<Channel, 16> channels {};
    std::array<Voice, 64> voices {}; // 原端口dwVoices=64。
    std::uint64_t frame {}, duration {};
    std::size_t eventIndex {};
    std::uint32_t repeats {};
    bool playing {};

    void ResetSequence()
    {
        frame = 0; eventIndex = 0; voices = {}; channels = {};
        for (auto& channel : channels) {
            channel.control[7] = 100; channel.control[10] = 64; channel.control[11] = 127;
            channel.control[100] = 127; channel.control[101] = 127;
        }
    }

    bool LoadDls(Bytes bytes)
    {
        if (!Tag(bytes, 0, "RIFF") || !Tag(bytes, 8, "DLS ") || bytes.size() < 12) { return false; }
        const auto riffLength = ReadLe(bytes, 4, 4);
        if (riffLength < 4 || riffLength > bytes.size() - 8) { return false; }
        Bytes root = bytes.subspan(12, riffLength - 4);
        std::vector<Chunk> chunks;
        if (!Chunks(root, chunks)) { return false; }
        const Chunk* pool = Find(root, chunks, "LIST", "wvpl");
        const Chunk* instruments = Find(root, chunks, "LIST", "lins");
        const Chunk* table = Find(root, chunks, "ptbl");
        if (pool == nullptr || instruments == nullptr || table == nullptr || table->data.size() < 8) { return false; }
        const auto cueSize = ReadLe(table->data, 0, 4), cueCount = ReadLe(table->data, 4, 4);
        if (cueSize < 8 || cueSize > table->data.size() || cueCount > (table->data.size() - cueSize) / 4) { return false; }
        Bytes waveBody = pool->data.subspan(4);
        std::vector<Chunk> waveLists;
        if (!Chunks(waveBody, waveLists)) { return false; }
        for (const auto& item : waveLists) {
            if (!Tag(waveBody, item.header, "LIST") || !Tag(item.data, 0, "wave")) { continue; }
            Bytes body = item.data.subspan(4);
            std::vector<Chunk> fields;
            if (!Chunks(body, fields)) { return false; }
            const Chunk* format = Find(body, fields, "fmt "), *data = Find(body, fields, "data");
            if (format == nullptr || data == nullptr || format->data.size() < 16 || ReadLe(format->data, 0, 2) != 1) { return false; }
            const auto bits = ReadLe(format->data, 14, 2);
            const auto count = ReadLe(format->data, 2, 2), frequency = ReadLe(format->data, 4, 4);
            if ((bits != 8 && bits != 16) || count == 0 || count > 2 || frequency == 0 || data->data.size() > INT_MAX) { return false; }
            SDL_AudioSpec input {bits == 8 ? SDL_AUDIO_U8 : SDL_AUDIO_S16LE, static_cast<int>(count), static_cast<int>(frequency)};
            SDL_AudioSpec output {SDL_AUDIO_F32, 2, static_cast<int>(frequency)};
            Uint8* pcm = nullptr; int length = 0;
            if (!SDL_ConvertAudioSamples(&input, data->data.data(), static_cast<int>(data->data.size()), &output, &pcm, &length)) { return false; }
            std::unique_ptr<Uint8, decltype(&SDL_free)> owned(pcm, &SDL_free);
            if (length <= 0 || length % (2 * sizeof(float)) != 0) { return false; }
            Wave wave;
            wave.frequency = frequency; wave.poolOffset = item.header;
            wave.stereo.resize(static_cast<std::size_t>(length) / sizeof(float));
            std::memcpy(wave.stereo.data(), pcm, length);
            if (const Chunk* wsmp = Find(body, fields, "wsmp"); wsmp != nullptr && !ReadSampleParameters(wsmp->data, wave.parameters)) { return false; }
            waves.push_back(std::move(wave));
        }
        Bytes instrumentBody = instruments->data.subspan(4);
        std::vector<Chunk> instrumentLists;
        if (!Chunks(instrumentBody, instrumentLists)) { return false; }
        for (const auto& item : instrumentLists) {
            if (!Tag(instrumentBody, item.header, "LIST") || !Tag(item.data, 0, "ins ")) { continue; }
            Bytes body = item.data.subspan(4);
            std::vector<Chunk> fields;
            if (!Chunks(body, fields)) { return false; }
            const Chunk* header = Find(body, fields, "insh"), *list = Find(body, fields, "LIST", "lrgn");
            if (header == nullptr || list == nullptr || header->data.size() < 12) { return false; }
            Articulation articulation;
            if (!ReadArticulation(body, fields, articulation)) { return false; }
            Bytes regionBody = list->data.subspan(4);
            std::vector<Chunk> regionLists;
            if (!Chunks(regionBody, regionLists)) { return false; }
            for (const auto& regionList : regionLists) {
                if (!Tag(regionBody, regionList.header, "LIST") || (!Tag(regionList.data, 0, "rgn ") && !Tag(regionList.data, 0, "rgn2"))) { continue; }
                Bytes regionData = regionList.data.subspan(4);
                std::vector<Chunk> parts;
                if (!Chunks(regionData, parts)) { return false; }
                const Chunk* range = Find(regionData, parts, "rgnh"), *link = Find(regionData, parts, "wlnk");
                if (range == nullptr || link == nullptr || range->data.size() < 12 || link->data.size() < 12) { return false; }
                const auto cue = ReadLe(link->data, 8, 4);
                if (cue >= cueCount) { return false; }
                const auto offset = ReadLe(table->data, cueSize + cue * 4, 4);
                const auto wave = std::find_if(waves.begin(), waves.end(), [offset](const Wave& value) { return value.poolOffset == offset; });
                if (wave == waves.end()) { return false; }
                Region region {};
                region.bank = ReadLe(header->data, 4, 4); region.program = ReadLe(header->data, 8, 4);
                region.wave = static_cast<std::uint32_t>(wave - waves.begin());
                region.keyLow = static_cast<std::uint16_t>(ReadLe(range->data, 0, 2)); region.keyHigh = static_cast<std::uint16_t>(ReadLe(range->data, 2, 2));
                region.velocityLow = static_cast<std::uint16_t>(ReadLe(range->data, 4, 2)); region.velocityHigh = static_cast<std::uint16_t>(ReadLe(range->data, 6, 2));
                region.articulation = articulation;
                if (!ReadArticulation(regionData, parts, region.articulation)) { return false; }
                if (const Chunk* wsmp = Find(regionData, parts, "wsmp"); wsmp != nullptr) {
                    region.ownParameters = true;
                    if (!ReadSampleParameters(wsmp->data, region.parameters)) { return false; }
                }
                const auto& parameters = region.ownParameters ? region.parameters : wave->parameters;
                if (parameters.loopStart > wave->stereo.size() / 2 || parameters.loopLength > wave->stereo.size() / 2 - parameters.loopStart) { return false; }
                regions.push_back(region);
            }
        }
        return !waves.empty() && !regions.empty();
    }

    bool LoadMidi(Bytes bytes)
    {
        if (bytes.size() < 14 || !Tag(bytes, 0, "MThd") || ReadBe(bytes, 4, 4) != 6 || ReadBe(bytes, 8, 2) > 1) { return false; }
        const auto trackCount = ReadBe(bytes, 10, 2), division = ReadBe(bytes, 12, 2);
        if (trackCount == 0 || division == 0 || (division & 0x8000u) != 0) { return false; }
        std::vector<Event> pending;
        std::size_t track = 14;
        for (std::uint32_t i = 0; i < trackCount; ++i) {
            if (!Tag(bytes, track, "MTrk") || bytes.size() - track < 8) { return false; }
            const auto length = ReadBe(bytes, track + 4, 4);
            if (length > bytes.size() - track - 8) { return false; }
            const std::size_t end = track + 8 + length;
            std::size_t at = track + 8; std::uint64_t tick = 0; std::uint8_t running = 0;
            auto variable = [&](std::uint32_t& value) {
                value = 0;
                for (unsigned n = 0; n < 4 && at < end; ++n) {
                    const auto part = bytes[at++]; value = (value << 7) | (part & 0x7Fu);
                    if ((part & 0x80u) == 0) { return true; }
                }
                return false;
            };
            while (at < end) {
                std::uint32_t delta;
                if (!variable(delta) || at == end) { return false; }
                tick += delta;
                std::uint8_t status = bytes[at];
                if ((status & 0x80u) != 0) { ++at; if (status < 0xF0u) { running = status; } }
                else { status = running; }
                if (status < 0x80u) { return false; }
                if (status == 0xFFu || status == 0xF0u || status == 0xF7u) {
                    std::uint8_t meta = 0;
                    if (status == 0xFFu) { if (at == end) { return false; } meta = bytes[at++]; }
                    std::uint32_t size;
                    if (!variable(size) || size > end - at) { return false; }
                    if (meta == 0x51u && size == 3) { pending.push_back({tick, 0, 0xFF, 0, 0, ReadBe(bytes, at, 3)}); }
                    at += size;
                    if (meta == 0x2Fu) { break; }
                    continue;
                }
                const unsigned kind = status & 0xF0u;
                const unsigned size = kind == 0xC0u || kind == 0xD0u ? 1 : 2;
                if (status >= 0xF0u || size > end - at || bytes[at] >= 128 || (size == 2 && bytes[at + 1] >= 128)) { return false; }
                pending.push_back({tick, 0, status, bytes[at], size == 1 ? std::uint8_t(0) : bytes[at + 1], 0});
                at += size;
            }
            pending.push_back({tick, 0, 0, 0, 0, 0}); // 原各track终点参与整段长度。
            track = end;
        }
        std::stable_sort(pending.begin(), pending.end(), [](const Event& first, const Event& second) { return first.tick < second.tick; });
        std::uint64_t lastTick = 0; double total = 0; std::uint32_t tempo = 500000;
        for (auto& event : pending) {
            total += static_cast<double>(event.tick - lastTick) * tempo * kSampleRate / (1000000.0 * division);
            event.frame = static_cast<std::uint64_t>(total); lastTick = event.tick;
            if (event.status == 0xFF) { if (event.tempo == 0) { return false; } tempo = event.tempo; }
        }
        if (pending.empty() || pending.back().frame == 0) { return false; }
        duration = pending.back().frame; events = std::move(pending); ResetSequence();
        return true;
    }

    void Apply(const Event& event)
    {
        if (event.status < 0x80 || event.status == 0xFF) { return; }
        const auto number = static_cast<std::uint8_t>(event.status & 15);
        auto& channel = channels[number]; const auto kind = event.status & 0xF0;
        if (kind == 0xB0) {
            channel.control[event.first] = event.second;
            if (event.first == 120) {
                // All Sound Off立即静音；不进入包络release或受延音踏板阻挡。
                for (auto& voice : voices) {
                    if (voice.region != nullptr && voice.channel == number) { voice = {}; }
                }
            } else if (event.first == 123) {
                // All Notes Off沿普通松键/CC64路径释放，保留已经开始的release。
                for (auto& voice : voices) {
                    if (voice.region != nullptr && voice.channel == number) { voice.keyUp = true; }
                }
            } else if (event.first == 100 || event.first == 101) {
                channel.nrpnSelected = false;
            } else if (event.first == 98 || event.first == 99) {
                channel.nrpnSelected = true;
            } else if ((event.first == 6 || event.first == 38)
                && !channel.nrpnSelected && channel.control[101] == 0) {
                // Data Entry只改所选参数的对应7位组；127/127与未知RPN不命中。
                switch (channel.control[100]) {
                case 0:
                    if (event.first == 6) { channel.bendSemitones = event.second; }
                    else { channel.bendCents = event.second; }
                    break;
                case 1:
                    channel.fineTuning = event.first == 6
                        ? static_cast<std::uint16_t>((channel.fineTuning & 0x7Fu)
                            | (std::uint16_t(event.second) << 7))
                        : static_cast<std::uint16_t>((channel.fineTuning & 0x3F80u) | event.second);
                    break;
                case 2:
                    if (event.first == 6) { channel.coarseTuning = event.second; }
                    break;
                default: break;
                }
            }
            return;
        }
        if (kind == 0xC0) { channel.program = event.first; return; }
        if (kind == 0xE0) { channel.bend = static_cast<std::uint16_t>(event.first | event.second << 7); return; }
        if (kind == 0x80 || (kind == 0x90 && event.second == 0)) {
            for (auto& voice : voices) {
                if (voice.region != nullptr && voice.channel == number && voice.key == event.first) { voice.keyUp = true; }
            }
            return;
        }
        if (kind != 0x90) { return; }
        const std::uint32_t bank = (number == 9 ? 0x80000000u : 0u) | (std::uint32_t(channel.control[0]) << 8) | channel.control[32];
        for (const auto& region : regions) {
            if (region.bank != bank || region.program != channel.program || event.first < region.keyLow || event.first > region.keyHigh || event.second < region.velocityLow || event.second > region.velocityHigh) { continue; }
            auto slot = std::find_if(voices.begin(), voices.end(), [](const Voice& value) { return value.region == nullptr; });
            if (slot == voices.end()) { slot = std::max_element(voices.begin(), voices.end(), [](const Voice& first, const Voice& second) { return first.age < second.age; }); }
            *slot = {}; slot->region = &region; slot->wave = &waves[region.wave];
            slot->parameters = region.ownParameters ? region.parameters : slot->wave->parameters;
            slot->channel = number; slot->key = event.first; slot->velocity = event.second;
        }
    }

    void Render(float* stereo, std::size_t count)
    {
        std::fill_n(stereo, count * 2, 0.0f);
        for (std::size_t i = 0; i < count && playing; ++i) {
            while (eventIndex < events.size() && events[eventIndex].frame <= frame) { Apply(events[eventIndex++]); }
            if (frame >= duration) {
                if (repeats == 0) { playing = false; break; }
                if (repeats != 0xFFFFFFFFu) { --repeats; }
                ResetSequence();
                while (eventIndex < events.size() && events[eventIndex].frame == 0) { Apply(events[eventIndex++]); }
            }
            for (auto& voice : voices) {
                if (voice.region == nullptr) { continue; }
                const auto& channel = channels[voice.channel]; const auto& art = voice.region->articulation;
                if (voice.keyUp && channel.control[64] < 64 && !voice.released) { voice.released = true; voice.releasedLevel = voice.envelope; voice.releaseAge = 0; }
                const double age = static_cast<double>(voice.age++) / kSampleRate;
                if (voice.released) {
                    if (art.release <= 0 || static_cast<double>(voice.releaseAge) / kSampleRate >= art.release) { voice.region = nullptr; continue; }
                    voice.envelope = voice.releasedLevel * (1.0 - static_cast<double>(voice.releaseAge++) / (kSampleRate * art.release));
                } else if (age < art.attack) { voice.envelope = age / art.attack; }
                else if (age < art.attack + art.decay) { voice.envelope = 1.0 - (1.0 - art.sustain) * (age - art.attack) / art.decay; }
                else { voice.envelope = art.sustain; }
                const auto& parameters = voice.parameters;
                const auto frames = voice.wave->stereo.size() / 2;
                if (parameters.loopLength != 0 && voice.cursor >= parameters.loopStart + parameters.loopLength) {
                    voice.cursor = parameters.loopStart + std::fmod(voice.cursor - parameters.loopStart, parameters.loopLength);
                }
                if (voice.cursor >= static_cast<double>(frames)) { voice.region = nullptr; continue; }
                const auto index = static_cast<std::size_t>(voice.cursor);
                std::size_t next = index + 1;
                if (parameters.loopLength != 0 && next >= parameters.loopStart + parameters.loopLength) { next = parameters.loopStart; }
                if (next >= frames) { next = index; }
                const double fraction = voice.cursor - static_cast<double>(index);
                const double gain = voice.envelope * std::pow(static_cast<double>(voice.velocity) / 127.0, 2.0)
                    * channel.control[7] / 127.0 * channel.control[11] / 127.0
                    * std::pow(10.0, -static_cast<double>(parameters.attenuation) / (65536.0 * 200.0));
                const double pan = (std::clamp)((static_cast<double>(channel.control[10]) - 64.0) / 64.0 + art.pan, -1.0, 1.0);
                for (std::size_t side = 0; side < 2; ++side) {
                    const auto& source = voice.wave->stereo;
                    const double value = source[index * 2 + side] * (1.0 - fraction) + source[next * 2 + side] * fraction;
                    stereo[i * 2 + side] += static_cast<float>(value * gain * (side == 0 ? (pan > 0 ? 1 - pan : 1) : (pan < 0 ? 1 + pan : 1)));
                }
                const double tuning = (static_cast<double>(channel.fineTuning) - 8192.0) / 8192.0
                    + static_cast<double>(channel.coarseTuning) - 64.0;
                const double bendRange = channel.bendSemitones + channel.bendCents / 100.0;
                const double pitch = voice.key - parameters.unity + (parameters.fine / 100.0) + tuning
                    + (static_cast<double>(channel.bend) - 8192.0) / 8192.0 * bendRange;
                voice.cursor += voice.wave->frequency / static_cast<double>(kSampleRate) * std::exp2(pitch / 12.0);
            }
            ++frame;
        }
    }
};

LegacyMidiSynth::LegacyMidiSynth() noexcept = default;
LegacyMidiSynth::~LegacyMidiSynth() = default;
bool LegacyMidiSynth::LoadBank(const char* path) noexcept
{
    Clear(); // 原455100先释放旧集合；离线重载失败也不保留旧bank/voice引用。
    try {
        std::size_t length = 0;
        std::unique_ptr<void, decltype(&SDL_free)> data(SDL_LoadFile(path, &length), &SDL_free);
        if (data == nullptr) { return false; }
        auto pending = std::make_unique<Impl>();
        if (!pending->LoadDls({static_cast<const std::uint8_t*>(data.get()), length})) { return false; }
        impl_ = std::move(pending); return true;
    } catch (const std::bad_alloc&) { return false; }
}
bool LegacyMidiSynth::LoadSequence(Bytes bytes) noexcept
{
    if (impl_ == nullptr) { return false; }
    UnloadSequence();
    try { return impl_->LoadMidi(bytes); } catch (const std::bad_alloc&) { return false; }
}
void LegacyMidiSynth::Start(std::uint32_t repeats) noexcept
{
    if (impl_ != nullptr && !impl_->events.empty()) { impl_->ResetSequence(); impl_->repeats = repeats; impl_->playing = true; }
}
void LegacyMidiSynth::Stop() noexcept { if (impl_ != nullptr) { impl_->playing = false; impl_->voices = {}; } }
void LegacyMidiSynth::UnloadSequence() noexcept
{
    if (impl_ != nullptr) {
        Stop(); impl_->duration = 0; impl_->eventIndex = 0;
        std::vector<Impl::Event>().swap(impl_->events);
    }
}
void LegacyMidiSynth::Clear() noexcept { impl_.reset(); }
bool LegacyMidiSynth::IsPlaying() const noexcept { return impl_ != nullptr && impl_->playing; }
std::uint64_t LegacyMidiSynth::FrameCount() const noexcept { return impl_ != nullptr ? impl_->duration : 0; }
void LegacyMidiSynth::Render(float* stereo, std::size_t frames) noexcept
{
    if (impl_ != nullptr) { impl_->Render(stereo, frames); } else { std::fill_n(stereo, frames * 2, 0.0f); }
}

} // namespace cg::platform
