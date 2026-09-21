#include "LegacyExternalKeyboardObject.hpp"

namespace cg::platform {

ExternalKeyboardSize* ExternalKeyboard::QuerySize(ExternalKeyboardSize* output)
{
    // 原 0x537DC4..0x537DD2：读宽、写宽后才读高、写高，不提前快照整个尺寸。
    output->width = size.width;
    output->height = size.height;
    return output;
}

ExternalKeyboardSize* QueryExternalKeyboardSizeVirtual_537DC0(
    ExternalKeyboard* object, ExternalKeyboardSize* output)
{
    return object->QuerySize(output);
}

char* GetExternalKeyboardResult_5354C0(ExternalKeyboard* object)
{
    // 原 0x5354C0 的结果地址属于同一对象；不复制、不清空，也不转换旧字节串。
    return object->result;
}

} // namespace cg::platform
