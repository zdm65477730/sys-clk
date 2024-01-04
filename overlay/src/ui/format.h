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

#include <cstdio>
#include <tesla.hpp>
using namespace tsl;

static inline std::string formatListFreqMhz(std::uint32_t mhz)
{
    if(mhz == 0)
    {
        return "DefaultFreqFarmatListText"_tr;
    }

    char buf[10];
    return std::string(buf, snprintf(buf, sizeof(buf), "%u Mhz", mhz));
}

static inline std::string formatListProfile(std::uint32_t profile)
{
    if(profile == 0)
    {
        return "DefaultFreqFarmatListText"_tr;
    }

	std::string profileFormat{" "};
    bool pretty = true;
    switch(profile)
    {
        case SysClkProfile_Docked:
            profileFormat = pretty ? "DockedPrettySysclkFormatProfileText"_tr : "DockedSysclkFormatProfileText"_tr;
            break;
        case SysClkProfile_Handheld:
            profileFormat = pretty ? "HandheldPrettySysclkFormatProfileText"_tr : "HandheldSysclkFormatProfileText"_tr;
            break;
        case SysClkProfile_HandheldCharging:
            profileFormat = pretty ? "HandheldChargingPrettySysclkFormatProfileText"_tr : "HandheldChargingSysclkFormatProfileText"_tr;
            break;
        case SysClkProfile_HandheldChargingUSB:
            profileFormat = pretty ? "HandheldChargingUSBPrettySysclkFormatProfileText"_tr : "HandheldChargingUSBSysclkFormatProfileText"_tr;
            break;
        case SysClkProfile_HandheldChargingOfficial:
            profileFormat = pretty ? "HandheldChargingOfficialPrettySysclkFormatProfileText"_tr : "HandheldChargingOfficialSysclkFormatProfileText"_tr;
            break;
        default:
            break;
    }

	return profileFormat;
}

static inline std::string formatListFreqHz(std::uint32_t hz) { return formatListFreqMhz(hz / 1000000); }
