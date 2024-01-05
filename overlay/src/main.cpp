/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#define TESLA_INIT_IMPL
#include <tesla.hpp>

#include "ui/gui/fatal_gui.h"
#include "ui/gui/main_gui.h"

using namespace tsl;

class AppOverlay : public tsl::Overlay
{
    public:
        AppOverlay() {
            std::string jsonStr = R"(
                {
                    "PluginName": "sys-clk",
                    "SysclkIpcNotRunningFatalGuiText": "sys-clk is not running.\n\n\nMake sure sys-clk is installed and enabled correctly.",
                    "SysclkIpcInitFailedFatalGuiText": "Can't connect to sys-clk.\n\n\nMake sure sys-clk is installed and enabled correctly.",
                    "SysclkIpcVersionMismatchFatalGuiText": "Overlay is incompatible with the running sys-clk version.\n\n\nMake sure everything is installed and up to date.",
                    "SysclkIpcSetOverrideFailedFatalGuiText": "sysclkIpcSetOverride",
                    "SysclkIpcSetProfilesFailedFatalGuiText": "sysclkIpcSetProfiles",
                    "SysclkIpcGetProfilesFailedFatalGuiText": "sysclkIpcGetProfiles",
                    "SysclkIpcGetCurrentContextFailedFatalGuiText": "sysclkIpcGetCurrentContext",
                    "DefaultFreqFarmatListText": "No change",
                    "AppIdMismatchFatalGuiText": "Application changed while editing,\n\n\nthe running application has been changed.",
                    "AppIDBaseMenuGuiText": "App ID:",
                    "ProfileBaseMenuGuiText": "Profile:",
                    "CPUBaseMenuGuiText": "CPU:",
                    "GPUBaseMenuGuiText": "GPU:",
                    "MemBaseMenuGuiText": "MEM:",
                    "ChipBaseMenuGuiText": "Chip:",
                    "PCBBaseMenuGuiText": "PCB:",
                    "SkinBaseMenuGuiText": "Skin:",
                    "BatteryPowerBaseMenuGuiText": "Battery Power",
                    "BatteryPowerNowBaseMenuGuiText": "Now:",
                    "BatteryPowerAvgBaseMenuGuiText": "Avg:",
                    "CPUPrettySysclkFormatModuleText": "CPU",
                    "CPUSysclkFormatModuleText": "cpu",
                    "GPUPrettySysclkFormatModuleText": "GPU",
                    "GPUSysclkFormatModuleText": "gpu",
                    "MEMPrettySysclkFormatModuleText": "Memory",
                    "MEMSysclkFormatModuleText": "mem",
                    "SOCPrettySysclkFormatThermalSensorText": "SOC",
                    "SOCSysclkFormatThermalSensorText": "soc",
                    "PCBPrettySysclkFormatThermalSensorText": "PCB",
                    "PCBSysclkFormatThermalSensorText": "pcb",
                    "SkinPrettySysclkFormatThermalSensorText": "Skin",
                    "SkinSysclkFormatThermalSensorText": "skin",
                    "DockedPrettySysclkFormatProfileText": "Docked",
                    "DockedSysclkFormatProfileText": "docked",
                    "HandheldPrettySysclkFormatProfileText": "Handheld",
                    "HandheldSysclkFormatProfileText": "handheld",
                    "HandheldChargingPrettySysclkFormatProfileText": "Charging",
                    "HandheldChargingSysclkFormatProfileText": "handheld_charging",
                    "HandheldChargingUSBPrettySysclkFormatProfileText": "USB Charger",
                    "HandheldChargingUSBSysclkFormatProfileText": "handheld_charging_usb",
                    "HandheldChargingOfficialPrettySysclkFormatProfileText": "Official Charger",
                    "HandheldChargingOfficialSysclkFormatProfileText": "handheld_charging_official",
                    "FatalErrorFatalGuiText": "Fatal error",
                    "ToggleListItemMainGuiText": "Enabled",
                    "SysclkIpcSetEnabledFailedFatalGuiText": "sysclkIpcSetEnabled",
                    "AppProfileListItemMainGuiText": "Edit profile",
                    "AdvanceProfileCategoryHeaderMainGuiText": "Advance profile",
                    "TempOverrideListItemMainGuiText": "Temp override",
                    "SysclkIpcSetConfigValuesFailedFatalGuiText": "sysclkIpcSetConfigValues failed",
                    "EnabledGlobalOverrideGuiToggleListItemText": "Yes",
                    "DisabledGlobalOverrideGuiToggleListItemText": "No",
                    "UncappedGPUGlobalOverrideGuiCustomToggleListItemText": "Uncapped GPU",
                    "OverrideMemToMaxGlobalOverrideGuiCustomToggleListItemText": "Override MEM to MAX",
                    "MinProfileGlobalOverrideGuiCustomToggleListItemText": "Min. profile"
                }
            )";
            std::string lanPath = std::string("sdmc:/switch/.overlays/lang/") + APPTITLE + "/";
            fsdevMountSdmc();
            tsl::hlp::doWithSmSession([&lanPath, &jsonStr]{
                tsl::tr::InitTrans(lanPath, jsonStr);
            });
            fsdevUnmountDevice("sdmc");
        }
        ~AppOverlay() {}

        virtual void exitServices() override {
            sysclkIpcExit();
        }

        virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
        {
            uint32_t apiVersion;
            smInitialize();

            tsl::hlp::ScopeGuard smGuard([] { smExit(); });

            if(!sysclkIpcRunning())
            {
                return initially<FatalGui>(
                    "SysclkIpcNotRunningFatalGuiText"_tr,
                    ""
                );
            }

            if(R_FAILED(sysclkIpcInitialize()) || R_FAILED(sysclkIpcGetAPIVersion(&apiVersion)))
            {
                return initially<FatalGui>(
                    "SysclkIpcInitFailedFatalGuiText"_tr,
                    ""
                );
            }

            if(SYSCLK_IPC_API_VERSION != apiVersion)
            {
                return initially<FatalGui>(
                    "SysclkIpcVersionMismatchFatalGuiText"_tr,
                    ""
                );
            }

            return initially<MainGui>();
        }
};

int main(int argc, char **argv)
{
    return tsl::loop<AppOverlay>(argc, argv);
}
