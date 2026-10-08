#include "cg/platform/LegacyWavesReverb.hpp"

#include <SDL3/SDL.h>
#include <atomic>
#include <bit>
#include <new>
#include <utility>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <initguid.h>
#include <dmo.h>
#include <dsound.h>
#include <uuids.h>

namespace cg::platform {

struct LegacyWavesReverb::Impl {
    enum class Operation { configure, reset, process, close };
    SDL_Mutex* control {}; // 串行Configure/Reset/Process，工作线程不取该锁。
    SDL_Semaphore* request {}; // 请求方发布operation后唤醒Run。
    SDL_Semaphore* completed {}; // Run发布result后完成一次同步借用。
    SDL_Thread* worker {}; // 唯一COM apartment与效果对象owner。
    int sampleRate {}; // 实际固定WAV频率，格式设置和100ns时间转换共用。
    bool opened {}; // 初次创建/格式/资源申请的真实结果，控制关闭时是否发请求。
    std::atomic<Operation> operation {Operation::close}; // release/acquire发布下面请求字段。
    std::atomic<bool> result {false}; // release/acquire发布请求结果及hasTail。
    std::int32_t decay {}; // 原455030参数0；仅configure请求消费。
    std::int32_t mix {}; // 原455030参数1；仅configure请求消费。
    float* pcm {}; // 同步借用请求方的双声道PCM，completed后清指针。
    std::size_t samples {}; // process输入float数，SDK按字节数消费。
    std::uint64_t startFrame {}; // 连续输出帧时点，重复段也不回退。
    bool zeroInput {}; // 最后一遍后用DMO_INPLACE_ZERO排尾音。
    bool hasTail {}; // Process返回S_FALSE，result同步后才交请求方读取。

    // 下列COM指针与所有调用都属于Run线程，不发布给请求方。
    static int SDLCALL Run(void* data)
    {
        auto& self = *static_cast<Impl*>(data);
        IMediaObject* media = nullptr;
        IMediaObjectInPlace* inPlace = nullptr;
        IDirectSoundFXWavesReverb* effect = nullptr;
        const HRESULT apartment = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        HRESULT status = apartment;
        if (SUCCEEDED(status)) {
            status = ::CoCreateInstance(GUID_DSFX_WAVES_REVERB, nullptr,
                CLSCTX_INPROC_SERVER, __uuidof(IMediaObject), reinterpret_cast<void**>(&media));
        }
        if (SUCCEEDED(status)) {
            status = media->QueryInterface(__uuidof(IMediaObjectInPlace), reinterpret_cast<void**>(&inPlace));
        }
        if (SUCCEEDED(status)) {
            status = media->QueryInterface(IID_IDirectSoundFXWavesReverb, reinterpret_cast<void**>(&effect));
        }
        WAVEFORMATEX format {};
        format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        format.nChannels = 2;
        format.nSamplesPerSec = static_cast<DWORD>(self.sampleRate);
        format.wBitsPerSample = 32;
        format.nBlockAlign = 8;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        DMO_MEDIA_TYPE type {};
        type.majortype = MEDIATYPE_Audio;
        type.subtype = MEDIASUBTYPE_IEEE_FLOAT;
        type.formattype = FORMAT_WaveFormatEx;
        type.bFixedSizeSamples = TRUE;
        type.lSampleSize = format.nBlockAlign;
        type.cbFormat = sizeof(format);
        type.pbFormat = reinterpret_cast<BYTE*>(&format);
        if (SUCCEEDED(status)) { status = media->SetInputType(0, &type, 0); }
        if (SUCCEEDED(status)) { status = media->SetOutputType(0, &type, 0); }
        if (SUCCEEDED(status)) { status = media->AllocateStreamingResources(); }
        DSFXWavesReverb configuredParameters {};
        bool configured = false;
        self.result.store(SUCCEEDED(status), std::memory_order_release);
        SDL_SignalSemaphore(self.completed);
        if (SUCCEEDED(status)) {
            for (;;) {
                SDL_WaitSemaphore(self.request);
                const Operation op = self.operation.load(std::memory_order_acquire);
                if (op == Operation::close) { break; }
                if (op == Operation::configure) {
                    const DSFXWavesReverb parameters {
                        0.0f, static_cast<float>(self.mix - 16),
                        static_cast<float>(self.decay * 100), std::bit_cast<float>(0x3A83126Fu)
                    };
                    status = effect->SetAllParameters(&parameters);
                    if (SUCCEEDED(status)) { configuredParameters = parameters; configured = true; }
                } else if (op == Operation::reset) {
                    status = media->Flush();
                    if (SUCCEEDED(status)) { status = media->FreeStreamingResources(); }
                    if (SUCCEEDED(status)) { status = media->AllocateStreamingResources(); }
                    if (SUCCEEDED(status) && configured) { status = effect->SetAllParameters(&configuredParameters); }
                } else {
                    const REFERENCE_TIME time = static_cast<REFERENCE_TIME>(
                        self.startFrame * 10000000u / static_cast<std::uint64_t>(self.sampleRate));
                    status = inPlace->Process(static_cast<ULONG>(self.samples * sizeof(float)),
                        reinterpret_cast<BYTE*>(self.pcm), time,
                        self.zeroInput ? DMO_INPLACE_ZERO : DMO_INPLACE_NORMAL);
                    self.hasTail = status == S_FALSE;
                }
                self.result.store(SUCCEEDED(status), std::memory_order_release);
                SDL_SignalSemaphore(self.completed);
            }
        }
        if (effect != nullptr) { effect->Release(); }
        if (inPlace != nullptr) { inPlace->Release(); }
        if (media != nullptr) { media->FreeStreamingResources(); media->Release(); }
        if (SUCCEEDED(apartment)) { ::CoUninitialize(); }
        return 0;
    }

    // 调用者持control，发布借用字段→请求→完成→结果，Run不回调音频流。
    bool Submit(const Operation op)
    {
        operation.store(op, std::memory_order_release);
        SDL_SignalSemaphore(request);
        SDL_WaitSemaphore(completed);
        return result.load(std::memory_order_acquire);
    }

    ~Impl()
    {
        if (worker != nullptr) {
            if (opened) {
                operation.store(Operation::close, std::memory_order_release);
                SDL_SignalSemaphore(request);
            }
            SDL_WaitThread(worker, nullptr);
        }
        if (completed != nullptr) { SDL_DestroySemaphore(completed); }
        if (request != nullptr) { SDL_DestroySemaphore(request); }
        if (control != nullptr) { SDL_DestroyMutex(control); }
    }
};

LegacyWavesReverb::LegacyWavesReverb() noexcept = default;
LegacyWavesReverb::~LegacyWavesReverb() = default;

bool LegacyWavesReverb::Open(const int sampleRate) noexcept
{
    Close();
    auto pending = std::unique_ptr<Impl>(new (std::nothrow) Impl);
    if (pending == nullptr) { return false; }
    pending->sampleRate = sampleRate;
    pending->control = SDL_CreateMutex();
    pending->request = SDL_CreateSemaphore(0);
    pending->completed = SDL_CreateSemaphore(0);
    if (pending->control == nullptr || pending->request == nullptr || pending->completed == nullptr) { return false; }
    pending->worker = SDL_CreateThread(&Impl::Run, "cg2d-waves-reverb", pending.get());
    if (pending->worker == nullptr) { return false; }
    SDL_WaitSemaphore(pending->completed);
    pending->opened = pending->result.load(std::memory_order_acquire);
    if (!pending->opened) { return false; }
    impl_ = std::move(pending);
    return true;
}

bool LegacyWavesReverb::Configure(const std::int32_t decay, const std::int32_t mix) noexcept
{
    if (impl_ == nullptr) { return false; }
    SDL_LockMutex(impl_->control);
    impl_->decay = decay;
    impl_->mix = mix;
    const bool result = impl_->Submit(Impl::Operation::configure);
    SDL_UnlockMutex(impl_->control);
    return result;
}

bool LegacyWavesReverb::Reset() noexcept
{
    if (impl_ == nullptr) { return false; }
    SDL_LockMutex(impl_->control);
    const bool result = impl_->Submit(Impl::Operation::reset);
    SDL_UnlockMutex(impl_->control);
    return result;
}

bool LegacyWavesReverb::Process(const std::span<float> stereo,
    const std::uint64_t startFrame, const bool zeroInput, bool& hasTail) noexcept
{
    if (impl_ == nullptr) { return false; }
    SDL_LockMutex(impl_->control);
    impl_->pcm = stereo.data();
    impl_->samples = stereo.size();
    impl_->startFrame = startFrame;
    impl_->zeroInput = zeroInput;
    const bool result = impl_->Submit(Impl::Operation::process);
    hasTail = impl_->hasTail;
    impl_->pcm = nullptr;
    SDL_UnlockMutex(impl_->control);
    return result;
}

void LegacyWavesReverb::Close() noexcept { impl_.reset(); }

} // namespace cg::platform
