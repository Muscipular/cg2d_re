#include "LegacyExternalKeyboardObject.hpp"

#include <new>

namespace cg::platform {

ExternalKeyboard* ExternalKeyboard::Delete(std::uint32_t flags)
{
    ExternalKeyboard* const object = this;
    // 原 0x535293：析构异常直接传播，不继续到 0x5352A0 的存储释放。
    object->~ExternalKeyboard();
    if ((flags & 1u) != 0) {
        ::operator delete(object);
    }
    return object;
}

// 原函数：0x00535290。
ExternalKeyboard* DeleteExternalKeyboardVirtual_535290(
    ExternalKeyboard* object, std::uint32_t flags)
{
    return object->Delete(flags);
}

ExternalKeyboardSize* ExternalKeyboard::QuerySize(ExternalKeyboardSize* output)
{
    // 原 0x537DC4..0x537DD2：读宽、写宽后才读高、写高，不提前快照整个尺寸。
    output->width = size.width;
    output->height = size.height;
    return output;
}

// 原函数：0x00537DC0。
ExternalKeyboardSize* QueryExternalKeyboardSizeVirtual_537DC0(
    ExternalKeyboard* object, ExternalKeyboardSize* output)
{
    return object->QuerySize(output);
}

// 原函数：0x005354C0。
char* GetExternalKeyboardResult_5354C0(ExternalKeyboard* object)
{
    // 原 0x5354C0 的结果地址属于同一对象；不复制、不清空，也不转换旧字节串。
    return object->result;
}

} // namespace cg::platform
