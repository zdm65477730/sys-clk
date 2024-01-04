/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam
    Copyright (C) 2019  m4xw

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "advanced_settings_tab.h"

#include "utils.h"

#include <sysclk/clocks.h>

AdvancedSettingsTab::AdvancedSettingsTab()
{
    // Get context
    SysClkContext context;
    Result rc = sysclkIpcGetCurrentContext(&context);

    if (R_FAILED(rc))
    {
        brls::Logger::error("application/manager/mainframe/advancedSettingsTab/sysclkIpcGetCurrentContextError"_i18n);
        errorResult("application/manager/mainframe/advancedSettingsTab/sysclkIpcGetCurrentContextErrorResult"_i18n, rc);
        brls::Application::crash("application/manager/mainframe/advancedSettingsTab/sysclkIpcGetCurrentContextCrash"_i18n);
        return;
    }

    // Create UI

    // Disclaimer
    this->addView(new brls::Label(brls::LabelStyle::REGULAR, "application/manager/mainframe/advancedSettingsTab/disclaimerLabel"_i18n, true));

    // Temporary overrides
    this->addView(new brls::Header("application/manager/mainframe/advancedSettingsTab/temporaryOverridesHeader"_i18n));

    // CPU
    brls::SelectListItem *cpuFreqListItem = createFreqListItem(SysClkModule_CPU, context.overrideFreqs[SysClkModule_CPU] / 1000000);
    cpuFreqListItem->getValueSelectedEvent()->subscribe([](int result){
        Result rc = result == 0 ?
            sysclkIpcRemoveOverride(SysClkModule_CPU) :
            sysclkIpcSetOverride(SysClkModule_CPU, sysclk_g_freq_table_cpu_hz[result - 1]);

        if (R_FAILED(rc))
        {
            brls::Logger::error("application/manager/mainframe/advancedSettingsTab/unableUpdateCPUOverrideError"_i18n);
            errorResult(result == 0 ? "application/manager/mainframe/advancedSettingsTab/sysclkIpcRemoveOverrideErrorResult"_i18n : "application/manager/mainframe/advancedSettingsTab/sysclkIpcSetOverrideErrorResult"_i18n,  rc);
            // TODO: Reset selected value
        }
    });

    // GPU
    brls::SelectListItem *gpuFreqListItem = createFreqListItem(SysClkModule_GPU, context.overrideFreqs[SysClkModule_GPU] / 1000000);
    gpuFreqListItem->getValueSelectedEvent()->subscribe([](int result){
        Result rc = result == 0 ?
            sysclkIpcRemoveOverride(SysClkModule_GPU) :
            sysclkIpcSetOverride(SysClkModule_GPU, sysclk_g_freq_table_gpu_hz[result - 1]);

        if (R_FAILED(rc))
        {
            brls::Logger::error("application/manager/mainframe/advancedSettingsTab/unableUpdateGPUOverrideError"_i18n);
            errorResult(result == 0 ? "application/manager/mainframe/advancedSettingsTab/sysclkIpcRemoveOverrideErrorResult"_i18n : "application/manager/mainframe/advancedSettingsTab/sysclkIpcSetOverrideErrorResult"_i18n,  rc);
            // TODO: Reset selected value
        }
    });

    // MEM
    brls::SelectListItem *memFreqListItem = createFreqListItem(SysClkModule_MEM, context.overrideFreqs[SysClkModule_MEM] / 1000000);
    memFreqListItem->getValueSelectedEvent()->subscribe([](int result)
    {
        Result rc = result == 0 ?
            sysclkIpcRemoveOverride(SysClkModule_MEM) :
            sysclkIpcSetOverride(SysClkModule_MEM, sysclk_g_freq_table_mem_hz[result - 1]);

        if (R_FAILED(rc))
        {
            brls::Logger::error("application/manager/mainframe/advancedSettingsTab/unableUpdateMEMOverrideError"_i18n);
            errorResult(result == 0 ? "application/manager/mainframe/advancedSettingsTab/sysclkIpcRemoveOverrideErrorResult"_i18n : "application/manager/mainframe/advancedSettingsTab/sysclkIpcSetOverrideErrorResult"_i18n,  rc);
            // TODO: Reset selected value
        }
    });

    this->addView(cpuFreqListItem);
    this->addView(gpuFreqListItem);
    this->addView(memFreqListItem);

    // Config
    this->addView(new brls::Header("application/manager/mainframe/advancedSettingsTab/configurationHeader"_i18n));

    // Logging
    // TODO: add a logger view and put the button to enter it here

    // Config entries
    // TODO: add constraints to the swkbd if possible (min / max)

    sysclkIpcGetConfigValues(&this->configValues);

    for (int i = 0; i < SysClkConfigValue_UncappedGPUEnabled; i++)
    {
        SysClkConfigValue config = (SysClkConfigValue) i;

        std::string label       = SysClkConfigValueToString(config, true);
        std::string description = this->getDescriptionForConfig(config);
        uint64_t defaultValue   = configValues.values[config];

        brls::IntegerInputListItem* configItem = new brls::IntegerInputListItem(label, defaultValue, label, description);

        configItem->setReduceDescriptionSpacing(true);

        configItem->getClickEvent()->subscribe([this, configItem, config](View* view)
        {
            try
            {
                int value = std::stoi(configItem->getValue());

                // Validate the value
                if (value < 0)
                {
                    brls::Application::notify("application/manager/mainframe/advancedSettingsTab/invalidNegativeValueConfigSaveFailedNotify"_i18n);
                    configItem->setValue(std::to_string(this->configValues.values[config]));
                    return;
                }

                uint64_t uvalue = (uint64_t) value;

                if (!sysclkValidConfigValue(config, uvalue))
                {
                    brls::Application::notify("application/manager/mainframe/advancedSettingsTab/invalidValueConfigSaveFailedNotify"_i18n);
                    configItem->setValue(std::to_string(this->configValues.values[config]));
                    return;
                }

                // Save the config
                this->configValues.values[config] = uvalue;
                sysclkIpcSetConfigValues(&this->configValues);

                brls::Application::notify("application/manager/mainframe/advancedSettingsTab/configSavedNotify"_i18n);
            }
            catch(const std::exception& e)
            {
                brls::Logger::error("application/manager/mainframe/advancedSettingsTab/unableParseConfigValueError"_i18n, configItem->getValue().c_str(), e.what());
            }
        });

        this->addView(configItem);
    }
    
    //loop for the added 5 custom configs
    for (int i = SysClkConfigValue_UncappedGPUEnabled; i < SysClkConfigValue_EnumMax; i++)
    {
        SysClkConfigValue config = (SysClkConfigValue) i;

        std::string label       = SysClkConfigValueToString(config, true);
        
        uint64_t defaultValue   = configValues.values[config];
        
        if (i==SysClkConfigValue_FakeProfileModeEnabled) {

            // Fake profile
            brls::SelectListItem *profileListItem = createProfileListItem(label, (uint32_t) defaultValue);
            profileListItem->getValueSelectedEvent()->subscribe([this,config,profileListItem](int result)
            {

                try
                {
                    
                    uint64_t uvalue = (uint64_t) sysclk_g_profile_table[result - 1];
                    
                    if (!sysclkValidConfigValue(config, uvalue))
                    {
                        brls::Application::notify("application/manager/mainframe/advancedSettingsTab/invalidValueConfigSaveFailedNotify"_i18n);
                        profileListItem->setValue(formatProfile((SysClkProfile)this->configValues.values[config]) );
                        return;
                    }

                    // Save the config
                    this->configValues.values[config] = uvalue;
                    sysclkIpcSetConfigValues(&this->configValues);

                    brls::Application::notify("application/manager/mainframe/advancedSettingsTab/configSavedNotify"_i18n);
                }
                catch(const std::exception& e)
                {
                    brls::Logger::error("application/manager/mainframe/advancedSettingsTab/unableParseConfigValueError"_i18n, profileListItem->getValue().c_str(), e.what());
                }
                
                
            });

            this->addView(profileListItem);

        } else {

            bool defValue = false;
            
            if(defaultValue==1) {
                defValue = true;
            }
        
            brls::ToggleListItem* configItem2 = new brls::ToggleListItem(label, defValue, "", "application/manager/mainframe/advancedSettingsTab/configItem2EnabledToggleListItemText"_i18n, "application/manager/mainframe/advancedSettingsTab/configItem2DisabledToggleListItemText"_i18n);
            
            configItem2->getClickEvent()->subscribe([this, configItem2, config](View* view)
            {
                try
                {
                    int value = 0;
                    std::string valuestring = configItem2->getValue();
                    
                    if(valuestring == "application/manager/mainframe/advancedSettingsTab/configItem2EnabledToggleListItemText"_i18n) {
                        value = 1;
                    }
                    
                    uint64_t uvalue = (uint64_t) value;

                    if (!sysclkValidConfigValue(config, uvalue))
                    {
                        brls::Application::notify("application/manager/mainframe/advancedSettingsTab/invalidValueConfigSaveFailedNotify"_i18n);
                        configItem2->setValue(std::to_string(this->configValues.values[config]));
                        return;
                    }

                    // Save the config
                    this->configValues.values[config] = uvalue;
                    sysclkIpcSetConfigValues(&this->configValues);

                    brls::Application::notify("application/manager/mainframe/advancedSettingsTab/configSavedNotify"_i18n);
                }
                catch(const std::exception& e)
                {
                    brls::Logger::error("application/manager/mainframe/advancedSettingsTab/unableParseConfigValueError"_i18n, configItem2->getValue().c_str(), e.what());
                }
            });

            this->addView(configItem2);
            
        }
        
    }
    
    // Notes
    brls::Label *notes = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "application/manager/mainframe/advancedSettingsTab/notesLable"_i18n,
        true
    );
    this->addView(notes);
    
}

std::string AdvancedSettingsTab::getDescriptionForConfig(SysClkConfigValue config)
{
    switch (config)
    {
        case SysClkConfigValue_CsvWriteIntervalMs:
            return "application/manager/mainframe/advancedSettingsTab/csvUpdateIntervalDescription"_i18n;
        case SysClkConfigValue_TempLogIntervalMs:
            return "application/manager/mainframe/advancedSettingsTab/temperatureLogIntervalDescription"_i18n;
        case SysClkConfigValue_PollingIntervalMs:
            return "application/manager/mainframe/advancedSettingsTab/pollingIntervalDescription"_i18n;
        default:
            return "";
    }
}
