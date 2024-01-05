/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "base_menu_gui.h"

#include "fatal_gui.h"
using namespace tsl;

BaseMenuGui::BaseMenuGui()
{
    this->context = nullptr;
    this->lastContextUpdate = 0;
    this->listElement = nullptr;
}

BaseMenuGui::~BaseMenuGui()
{
    if(this->context)
    {
        delete this->context;
    }
}

void BaseMenuGui::preDraw(tsl::gfx::Renderer* renderer)
{
    BaseGui::preDraw(renderer);
    if(this->context)
    {
        char buf[32];

        std::uint32_t y = 85;

        renderer->drawString("AppIDBaseMenuGuiText"_tr.c_str(), false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);
        snprintf(buf, sizeof(buf), "%016lX", context->applicationId);
        renderer->drawString(buf, false, 81, y, SMALL_TEXT_SIZE, VALUE_COLOR);

        renderer->drawString("ProfileBaseMenuGuiText"_tr.c_str(), false, 246, y, SMALL_TEXT_SIZE, DESC_COLOR);

        std::string profileFormat{" "};
        bool pretty = true;
        switch(context->profile)
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
        renderer->drawString(profileFormat.c_str(), false, 302, y, SMALL_TEXT_SIZE, VALUE_COLOR);

        y += 30;

        static struct
        {
            SysClkModule m;
            std::uint32_t x;
        } freqOffsets[SysClkModule_EnumMax] = {
            { SysClkModule_CPU, 61 },
            { SysClkModule_GPU, 204 },
            { SysClkModule_MEM, 342 },
        };

        for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
        {
            std::uint32_t hz = this->context->freqs[freqOffsets[i].m];

            snprintf(buf, sizeof(buf), "%u.%u MHz", hz / 1000000, hz / 100000 - hz / 1000000 * 10);
            renderer->drawString(buf, false, freqOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }
        renderer->drawString("CPUBaseMenuGuiText"_tr.c_str(), false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("GPUBaseMenuGuiText"_tr.c_str(), false, 162, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("MemBaseMenuGuiText"_tr.c_str(), false, 295, y, SMALL_TEXT_SIZE, DESC_COLOR);

        y += 25;

        for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
        {
            std::uint32_t hz = this->context->realFreqs[freqOffsets[i].m];
            snprintf(buf, sizeof(buf), "%u.%u MHz", hz / 1000000, hz / 100000 - hz / 1000000 * 10);
            renderer->drawString(buf, false, freqOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }

        y += 25;

        static struct
        {
            SysClkThermalSensor s;
            std::uint32_t x;
        } tempOffsets[SysClkModule_EnumMax] = {
            { SysClkThermalSensor_SOC, 61 },
            { SysClkThermalSensor_PCB, 204 },
            { SysClkThermalSensor_Skin, 342 },
        };

        renderer->drawString("ChipBaseMenuGuiText"_tr.c_str(), false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("PCBBaseMenuGuiText"_tr.c_str(), false, 166, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("SkinBaseMenuGuiText"_tr.c_str(), false, 303, y, SMALL_TEXT_SIZE, DESC_COLOR);

        for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
        {
            std::uint32_t millis = this->context->temps[tempOffsets[i].s];
            snprintf(buf, sizeof(buf), "%u.%u °C", millis / 1000, (millis - millis / 1000 * 1000) / 100);

            renderer->drawString(buf, false, tempOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }

        y += 30;

        static struct
        {
            SysClkPowerSensor s;
            std::uint32_t x;
        } powerOffsets[SysClkPowerSensor_EnumMax] = {
            { SysClkPowerSensor_Now, 204 },
            { SysClkPowerSensor_Avg, 342 },
        };

        renderer->drawString("BatteryPowerBaseMenuGuiText"_tr.c_str(), false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);

        renderer->drawString("BatteryPowerNowBaseMenuGuiText"_tr.c_str(), false, 160, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("BatteryPowerAvgBaseMenuGuiText"_tr.c_str(), false, 304, y, SMALL_TEXT_SIZE, DESC_COLOR);
        for(unsigned int i = 0; i < SysClkPowerSensor_EnumMax; i++)
        {
            std::uint32_t mw = this->context->power[powerOffsets[i].s];
            snprintf(buf, sizeof(buf), "%d mW", mw);
            renderer->drawString(buf, false, powerOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);

        }
    }
}

void BaseMenuGui::refresh()
{
    std::uint64_t ticks = armGetSystemTick();

    if(armTicksToNs(ticks - this->lastContextUpdate) > 500000000UL)
    {
        this->lastContextUpdate = ticks;
        if(!this->context)
        {
            this->context = new SysClkContext;
        }

        Result rc = sysclkIpcGetCurrentContext(this->context);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("SysclkIpcGetCurrentContextFailedFatalGuiText"_tr, rc);
            return;
        }
    }
}

tsl::elm::Element* BaseMenuGui::baseUI()
{
    tsl::elm::List* list = new tsl::elm::List();
    this->listElement = list;
    this->listUI();

    return list;
}
