/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <switch.h>

Result tmp451Initialize(void);
void tmp451Exit(void);
s32 tmp451TempPcb(void);
s32 tmp451TempSoc(void);

#ifdef __cplusplus
}
#endif
