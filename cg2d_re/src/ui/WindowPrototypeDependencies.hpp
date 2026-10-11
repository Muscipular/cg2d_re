#pragma once

#include "WindowRuntimeState.hpp"

namespace cg::ui::detail {

// 原57C130指向的元素数组；实际初值和回调绑定位于WindowPrototypes.cpp，本头声明供窗口调用侧使用。
extern WindowElementPrototype gWindowElements55ED60[11];
extern WindowElementPrototype gWindowElements55EEC0[2];
extern WindowElementPrototype gWindowElements55F0A0[24];
extern WindowElementPrototype gWindowElements55F3A0[7];
extern WindowElementPrototype gWindowElements55F480[2];
extern WindowElementPrototype gWindowElements55F5C0[14];
extern WindowElementPrototype gWindowElements55FD60[73];
extern WindowElementPrototype gWindowElements560840[43];
extern WindowElementPrototype gWindowElements560F50[37];
extern WindowElementPrototype gWindowElements561558[34];
extern WindowElementPrototype gWindowElements561B78[30];
extern WindowElementPrototype gWindowElements5620E0[31];
extern WindowElementPrototype gWindowElements562620[15];
extern WindowElementPrototype gWindowElements562930[14];
extern WindowElementPrototype gWindowElements562CD8[24];
extern WindowElementPrototype gWindowElements5630F0[8];
extern WindowElementPrototype gWindowElements563500[259];
extern WindowElementPrototype gWindowElements5659C0[66];
extern WindowElementPrototype gWindowElements566200[46];
extern WindowElementPrototype gWindowElements566810[51];
extern WindowElementPrototype gWindowElements567028[71];
extern WindowElementPrototype gWindowElements567AE8[82];
extern WindowElementPrototype gWindowElements5689C8[32];
extern WindowElementPrototype gWindowElements568F78[25];
extern WindowElementPrototype gWindowElements56AF50[7];
extern WindowElementPrototype gWindowElements56B2B8[20];
extern WindowElementPrototype gWindowElements56B710[14];
extern WindowElementPrototype gWindowElements56B8D8[2];
extern WindowElementPrototype gWindowElements56B9B8[66];
extern WindowElementPrototype gWindowElements56C2C8[4];
extern WindowElementPrototype gWindowElements56C430[22];
extern WindowElementPrototype gWindowElements56C800[8];
extern WindowElementPrototype gWindowElements56C9B8[2];
extern WindowElementPrototype gWindowElements56CA88[19];
extern WindowElementPrototype gWindowElements56CCD0[1];
extern WindowElementPrototype gWindowElements56CDB0[19];
extern WindowElementPrototype gWindowElements56D0B0[19];
extern WindowElementPrototype gWindowElements56D3E8[34];
extern WindowElementPrototype gWindowElements56D838[3];
extern WindowElementPrototype gWindowElements56DB38[111];
extern WindowElementPrototype gWindowElements56F860[29];
extern WindowElementPrototype gWindowElements56FD70[21];
extern WindowElementPrototype gWindowElements5701F8[9];
extern WindowElementPrototype gWindowElements570318[13];
extern WindowElementPrototype gWindowElements5706D0[34];
extern WindowElementPrototype gWindowElements570CB0[20];
extern WindowElementPrototype gWindowElements570FD8[2];
extern WindowElementPrototype gWindowElements571160[38];
extern WindowElementPrototype gWindowElements571740[6];
extern WindowElementPrototype gWindowElements5719E8[26];
extern WindowElementPrototype gWindowElements571F18[76];
extern WindowElementPrototype gWindowElements572AF8[20];
extern WindowElementPrototype gWindowElements572F90[37];
extern WindowElementPrototype gWindowElements5735C8[36];
extern WindowElementPrototype gWindowElements573D80[66];
extern WindowElementPrototype gWindowElements5748E8[107];
extern WindowElementPrototype gWindowElements575988[70];
extern WindowElementPrototype gWindowElements576588[70];
extern WindowElementPrototype gWindowElements576F90[15];
extern WindowElementPrototype gWindowElements577298[32];
extern WindowElementPrototype gWindowElements5777A8[162];
extern WindowElementPrototype gWindowElements578CC8[71];
extern WindowElementPrototype gWindowElements579710[22];
extern WindowElementPrototype gWindowElements579B28[16];
extern WindowElementPrototype gWindowElements57A070[21];
extern WindowElementPrototype gWindowElements57A450[71];
extern WindowElementPrototype gWindowElements57AEC0[25];
extern WindowElementPrototype gWindowElements57B508[48];
extern WindowElementPrototype gWindowElements57BB08[43];

// 主原型中的真实回调依赖；本声明只约束窗口调用侧，不替代各本体取证。
// 原函数：0x0046FBA0。
std::int32_t Sub_46FBA0(std::int32_t value);
// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
std::int32_t Sub_470A80(std::int32_t value);
// 原函数：0x00470AB0。
std::int32_t Sub_470AB0(std::int32_t value);
// HWT01：窗口6按原显示/复制阶段消费共享悬停文本，颜色只读低BYTE，尾部清槽并返S32 1。
// 原函数：0x004731E0。
std::int32_t Sub_4731E0(std::int32_t value);
// 原函数：0x00474310。
void Sub_474310();
// 原函数：0x00474360。
void Sub_474360();
// 原函数：0x004744A0。
void Sub_4744A0();
// 原函数：0x004744F0。
void Sub_4744F0();
// 原函数：0x00474540。
std::int32_t Sub_474540(std::int32_t value);
// 原函数：0x00475DF0。
std::int32_t Sub_475DF0(std::int32_t value);
// 原函数：0x004774C0。
// 关联原入口：0x0047752C（共享尾已并入本函数正文）。
std::int32_t Sub_4774C0(std::int32_t value);
// 原函数：0x00478320。
std::int32_t Sub_478320(std::int32_t value);
// 原函数：0x0047A620。
std::int32_t Sub_47A620(std::int32_t value);
// 原函数：0x0047A7A0。
std::int32_t Sub_47A7A0(std::int32_t value);
void Sub_47B0E0();
// 原函数：0x00481010。
void Sub_481010();
// 原函数：0x004810A0。
std::int32_t Sub_4810A0(std::int32_t value);
// 原函数：0x00482000。
std::int32_t Sub_482000(std::int32_t value);
void Sub_485690();
// 原函数：0x00486040。
std::int32_t Sub_486040(std::int32_t value);
// 原函数：0x00487660。
void Sub_487660();
// 原函数：0x00487AE0。
std::int32_t Sub_487AE0(std::int32_t value);
// 原函数：0x00487B80。
std::int32_t Sub_487B80(std::int32_t value);
// 原函数：0x00488510。
void Sub_488510();
// 原函数：0x00488990。
std::int32_t Sub_488990(std::int32_t value);
// 原函数：0x00488B10。
std::int32_t Sub_488B10(std::int32_t value);
// 原函数：0x0048A8C0。
std::int32_t Sub_48A8C0(std::int32_t value);
// 原函数：0x0048C1B0。
// 关联原入口：0x0048C1CF（共享尾已并入本函数正文）。
std::int32_t Sub_48C1B0(std::int32_t value);
// 原函数：0x0048D9A0。
std::int32_t Sub_48D9A0(std::int32_t value);
// 原函数：0x0048DAB0。
std::int32_t Sub_48DAB0(std::int32_t value);
// 原函数：0x0048DBB0。
std::int32_t Sub_48DBB0(std::int32_t value);
// 原函数：0x0048DCB0。
std::int32_t Sub_48DCB0(std::int32_t value);
// 原函数：0x0048DCC0。
void Sub_48DCC0();
// 原函数：0x0048DD00。
void Sub_48DD00();
// 原函数：0x0048DD40。
void Sub_48DD40();
// 原函数：0x004923F0。
std::int32_t Sub_4923F0(std::int32_t value);
// 原函数：0x00492410。
std::int32_t Sub_492410(std::int32_t value);
// 原函数：0x00493D60。
std::int32_t Sub_493D60(std::int32_t value);
// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
std::int32_t Sub_493DB0(std::int32_t value);
// 原函数：0x00493E10。
void Sub_493E10();
// 原函数：0x00495130。
std::int32_t Sub_495130(std::int32_t value);
// 原函数：0x004951C0。
std::int32_t Sub_4951C0(std::int32_t value);
// 原函数：0x00495D70。
std::int32_t Sub_495D70(std::int32_t value);
// 原函数：0x00495F50。
std::int32_t Sub_495F50(std::int32_t value);
// 原函数：0x00496B90。
std::int32_t Sub_496B90(std::int32_t value);
// 原函数：0x00496D50。
std::int32_t Sub_496D50(std::int32_t value);
// 原函数：0x004991C0。
std::int32_t Sub_4991C0(std::int32_t value);
// 原函数：0x004996C0。
std::int32_t Sub_4996C0(std::int32_t value);
// 原函数：0x00499790。
std::int32_t Sub_499790(std::int32_t value);
// 原函数：0x00499990。
std::int32_t Sub_499990(std::int32_t value);
// 原函数：0x004999D0。
std::int32_t Sub_4999D0(std::int32_t value);
// 原函数：0x0049BD40。
std::int32_t Sub_49BD40(std::int32_t value);
// 原函数：0x0049C710。
std::int32_t Sub_49C710(std::int32_t value);
// 原函数：0x0049CAD0。
std::int32_t Sub_49CAD0(std::int32_t value);
// 原函数：0x0049CDF0。
void Sub_49CDF0();
// 原函数：0x0049FD20。
std::int32_t Sub_49FD20(std::int32_t value);
// 原函数：0x0049FE40。
std::int32_t Sub_49FE40(std::int32_t value);
// 原函数：0x0049FF70。
void Sub_49FF70();
// 原函数：0x004A0970。
std::int32_t Sub_4A0970(std::int32_t value);
// 原函数：0x004A09B0。
std::int32_t Sub_4A09B0(std::int32_t value);
// 原函数：0x004A4180。
std::int32_t Sub_4A4180(std::int32_t value);
// 原函数：0x004A41A0。
std::int32_t Sub_4A41A0(std::int32_t value);
// 原函数：0x004A4620。
std::int32_t Sub_4A4620(std::int32_t value);
// 原函数：0x004A4650。
std::int32_t Sub_4A4650(std::int32_t value);
// 原函数：0x004A53F0。
std::int32_t Sub_4A53F0(std::int32_t value);
// 原函数：0x004A5650。
void Sub_4A5650();
// 原函数：0x004A56F0。
std::int32_t Sub_4A56F0(std::int32_t value);
// 原函数：0x004A6DD0。
std::int32_t Sub_4A6DD0(std::int32_t value); // CWC01：见聊天窗口控制完整合同。
// 原函数：0x004A8240。
std::int32_t Sub_4A8240(std::int32_t value);
// CFD01：窗口35/57共用C9A96C绘制门禁与原返1，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A82C0。
std::int32_t Sub_4A82C0(std::int32_t value);
// 原函数：0x004A83D0。
void Sub_4A83D0();
// 原函数：0x004A9020。
std::int32_t Sub_4A9020(std::int32_t value);
// 原函数：0x004A9160。
std::int32_t Sub_4A9160(std::int32_t value);
// 原函数：0x004AC0F0。
void Sub_4AC0F0();
// 原函数：0x004AC140。
std::int32_t Sub_4AC140(std::int32_t value);
// 原函数：0x004AC1B0。
// 关联原入口：0x004AC1EC（共享尾已并入本函数正文）。
std::int32_t Sub_4AC1B0(std::int32_t value);
// 原函数：0x004AD780。
void Sub_4AD780();
// 原函数：0x004AD800。
std::int32_t Sub_4AD800(std::int32_t value);
// 原函数：0x004AD9F0。
std::int32_t Sub_4AD9F0(std::int32_t value);
// 原函数：0x004AE5F0。
std::int32_t Sub_4AE5F0(std::int32_t value);
// 原函数：0x004AEAE0。
std::int32_t Sub_4AEAE0(std::int32_t value);
// 原函数：0x004AEC10。
std::int32_t Sub_4AEC10(std::int32_t value);
// 原函数：0x004B4B00。
std::int32_t Sub_4B4B00(std::int32_t value);
// 原函数：0x004B4B70。
void Sub_4B4B70();
// 原函数：0x004B4B90。
std::int32_t Sub_4B4B90(std::int32_t value);
// 原函数：0x004B5AD0。
std::int32_t Sub_4B5AD0(std::int32_t value);
// 原函数：0x004B5B50。
void Sub_4B5B50();
// 原函数：0x004B6DC0。
void Sub_4B6DC0();
// 原函数：0x004B9310。
std::int32_t Sub_4B9310(std::int32_t value);
// 原函数：0x004B99D0。
std::int32_t Sub_4B99D0(std::int32_t value);
// 原函数：0x004BD500。
void Sub_4BD500();
// 原函数：0x004BF7C0。
std::int32_t Sub_4BF7C0(std::int32_t value);
// 原函数：0x004BF950。
std::int32_t Sub_4BF950(std::int32_t value);
// 原函数：0x004C1390。
void Sub_4C1390();
// 原函数：0x004C1C50。
std::int32_t Sub_4C1C50(std::int32_t value);
// 原函数：0x004C1EC0。
std::int32_t Sub_4C1EC0(std::int32_t value);
// 原函数：0x004C3530。
void Sub_4C3530();
// 原函数：0x004C3F00。
std::int32_t Sub_4C3F00(std::int32_t value);
// 原函数：0x004C4180。
std::int32_t Sub_4C4180(std::int32_t value);
// 原函数：0x004C5850。
std::int32_t Sub_4C5850(std::int32_t value);
// 原函数：0x004C5B70。
std::int32_t Sub_4C5B70(std::int32_t value);
// 原函数：0x004C5C00。
void Sub_4C5C00();
// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
std::int32_t Sub_4C94B0(std::int32_t value);
// 原函数：0x004CBD90。
std::int32_t Sub_4CBD90(std::int32_t value);
// 原函数：0x004CC6C0。
std::int32_t Sub_4CC6C0(std::int32_t value);

} // namespace cg::ui::detail
