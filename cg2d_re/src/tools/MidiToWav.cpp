// 仅离线转换原固定资源；不打开音频设备，不进入客户端，不加入客户端链接。
#include "LegacyMidiSynth.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <Windows.h>

namespace {
std::string Utf8(const wchar_t* path)
{
    const int size = ::WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
    if (size == 0) { return ""; }
    std::string result(static_cast<std::size_t>(size), '\0');
    if (::WideCharToMultiByte(CP_UTF8, 0, path, -1, result.data(), size, nullptr, nullptr) == 0) { return ""; }
    return result;
}
bool Word(SDL_IOStream* io, std::uint32_t value)
{
    const std::array<std::uint8_t, 4> bytes {static_cast<std::uint8_t>(value),
        static_cast<std::uint8_t>(value >> 8), static_cast<std::uint8_t>(value >> 16), static_cast<std::uint8_t>(value >> 24)};
    return SDL_WriteIO(io, bytes.data(), bytes.size()) == bytes.size();
}
}
int wmain(int argc, wchar_t** argv)
{
    if (argc != 4) { std::fprintf(stderr, "用法：midi_to_wav 输入MIDI 系统gm.dls 输出WAV\n"); return 1; }
    const auto input = Utf8(argv[1]), bank = Utf8(argv[2]), output = Utf8(argv[3]);
    std::size_t length = 0;
    std::unique_ptr<void, decltype(&SDL_free)> bytes(SDL_LoadFile(input.c_str(), &length), &SDL_free);
    cg::platform::LegacyMidiSynth synth;
    if (bytes == nullptr || !synth.LoadBank(bank.c_str())
        || !synth.LoadSequence({static_cast<const std::uint8_t*>(bytes.get()), length})) {
        std::fprintf(stderr, "转换失败：音色或MIDI资源无效，%s\n", SDL_GetError()); return 2;
    }
    const auto frames = synth.FrameCount();
    if (frames == 0 || frames > (UINT32_MAX - 48u) / 8u) { std::fprintf(stderr, "WAV帧数超出RIFF范围\n"); return 3; }
    const auto dataSize = static_cast<std::uint32_t>(frames * 8);
    SDL_IOStream* io = SDL_IOFromFile(output.c_str(), "wb");
    if (io == nullptr) { std::fprintf(stderr, "无法写入WAV：%s\n", SDL_GetError()); return 4; }
    // IEEE float32/双声道/22050Hz；保留现有合成PCM，不作整数位深量化或逐样本裁剪。
    bool written = SDL_WriteIO(io, "RIFF", 4) == 4 && Word(io, dataSize + 48)
        && SDL_WriteIO(io, "WAVEfmt ", 8) == 8 && Word(io, 16) && Word(io, 0x00020003)
        && Word(io, synth.kSampleRate) && Word(io, synth.kSampleRate * 8)
        && Word(io, 0x00200008) && SDL_WriteIO(io, "fact", 4) == 4 && Word(io, 4)
        && Word(io, static_cast<std::uint32_t>(frames)) && SDL_WriteIO(io, "data", 4) == 4 && Word(io, dataSize);
    std::array<float, 1024> pcm {};
    std::uint64_t rendered = 0, nonzero = 0;
    double peak = 0, power = 0;
    synth.Start(0);
    while (written && rendered < frames) {
        const auto count = static_cast<std::size_t>((std::min)(frames - rendered, std::uint64_t(512)));
        synth.Render(pcm.data(), count);
        for (std::size_t i = 0; i < count * 2; ++i) {
            if (!std::isfinite(pcm[i])) { written = false; break; }
            peak = (std::max)(peak, static_cast<double>(std::abs(pcm[i])));
            power += static_cast<double>(pcm[i]) * pcm[i];
            if (pcm[i] != 0) { ++nonzero; }
        }
        rendered += count;
    }
    // 离线统一衰减避免超过数字满幅；声部/声像和相对音量关系保持，不对每段单独压缩。
    const double gain = peak > 0.98 ? 0.98 / peak : 1.0;
    synth.Start(0);
    rendered = 0;
    while (written && rendered < frames) {
        const auto count = static_cast<std::size_t>((std::min)(frames - rendered, std::uint64_t(512)));
        synth.Render(pcm.data(), count);
        for (std::size_t i = 0; i < count * 2; ++i) { pcm[i] = static_cast<float>(pcm[i] * gain); }
        written = SDL_WriteIO(io, pcm.data(), count * 2 * sizeof(float)) == count * 2 * sizeof(float);
        rendered += count;
    }
    synth.Clear(); // 离线音色/序列owner完成最后一次PCM生产后释放，承接原loader清理职责。
    const bool closed = SDL_CloseIO(io);
    if (!written || !closed || nonzero == 0) { std::fprintf(stderr, "WAV写入失败或没有有效声音\n"); return 5; }
    std::printf("frames=%llu rate=%d channels=2 float_bits=32 seconds=%.6f source_peak=%.9f gain=%.9f peak=%.9f rms=%.9f nonzero_samples=%llu\n",
        static_cast<unsigned long long>(frames), synth.kSampleRate, static_cast<double>(frames) / synth.kSampleRate,
        peak, gain, peak * gain, std::sqrt(power / (frames * 2)) * gain, static_cast<unsigned long long>(nonzero));
    return 0;
}
