#pragma once

// 迁自twcg_hook/include/vm.h：保留Themida封装，TMD由CMake控制，Debug／MY_DEBUG不生成保护标记。
// 成对标记应直接放在需保护的业务区域内，不能用普通函数包装替代。
#if defined(TMD) && !defined(MY_DEBUG)
#include <ThemidaSDK.h>

#undef VM_START
#undef VM_END
#define VM_START VM_EAGLE_RED_START
#define VM_END VM_EAGLE_RED_END
#define VM_LIGHT_START VM_FALCON_TINY_START
#define VM_LIGHT_END VM_FALCON_TINY_END
#define VM_HARD_START VM_PUMA_WHITE_START
#define VM_HARD_END VM_PUMA_WHITE_END
#define VM_HARD2_START VM_PUMA_RED_START
#define VM_HARD2_END VM_PUMA_RED_END

#else

#define VM_START
#define VM_END
#define VM_LIGHT_START
#define VM_LIGHT_END
#define VM_HARD_START
#define VM_HARD_END
#define VM_HARD2_START
#define VM_HARD2_END
#define STR_ENCRYPT_START
#define STR_ENCRYPT_END
#define VM_PUMA_RED_START
#define VM_PUMA_RED_END
#define VM_DOLPHIN_WHITE_START
#define VM_DOLPHIN_WHITE_END
#define VM_TIGER_RED_START
#define VM_TIGER_RED_END
#define VM_EAGLE_WHITE_START
#define VM_EAGLE_WHITE_END
#define VM_PUMA_WHITE_START
#define VM_PUMA_WHITE_END
#define VM_FALCON_TINY_START
#define VM_FALCON_TINY_END

#endif
