/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "global_override_gui.h"

#include "fatal_gui.h"
#include "../format.h"

GlobalOverrideGui::GlobalOverrideGui()
{
    for(std::uint16_t m = 0; m < SysClkModule_EnumMax; m++)
    {
        this->listItems[m] = nullptr;
        this->listHz[m] = 0;
    }
    
    for(std::uint16_t m = 0; m < SysClkConfigValue_EnumMax; m++)
    {
        this->ToggleListItems[m] = nullptr;
    }
}

void GlobalOverrideGui::openFreqChoiceGui(SysClkModule module, std::uint32_t* hzList)
{
    tsl::changeTo<FreqChoiceGui>(this->context->overrideFreqs[module], hzList, [this, module](std::uint32_t hz) {
        Result rc = sysclkIpcSetOverride(module, hz);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcSetOverride", rc);
            return false;
        }

        this->lastContextUpdate = armGetSystemTick();
        this->context->overrideFreqs[module] = hz;

        return true;
    });
}

void GlobalOverrideGui::addModuleListItem(SysClkModule module, std::uint32_t* hzList)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(sysclkFormatModule(module, true));
    listItem->setValue(formatListFreqMhz(0));

    listItem->setClickListener([this, module, hzList](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            this->openFreqChoiceGui(module, hzList);
            return true;
        }

        return false;
    });

    this->listElement->addItem(listItem);
    this->listItems[module] = listItem;
}

void GlobalOverrideGui::addModuleListToggleListItem(int configNumber,std::string shortLabel)
{
       
    sysclkIpcGetConfigValues(&this->configValues);

    SysClkConfigValue config = (SysClkConfigValue) configNumber;
    
    uint64_t defaultValue   = configValues.values[config];
    
    bool defValue = false;
    
    if(defaultValue==1) {
        defValue = true;
    }

    this->enabledToggle = new tsl::elm::ToggleListItem(shortLabel, defValue);

    enabledToggle->setStateChangedListener([this,config](bool state) {
            
            int value = 0;
            
            if(state) {
                value = 1;
            }
            
            uint64_t uvalue = (uint64_t) value;
            
            // Save the config
            this->configValues.values[config] = uvalue;
            sysclkIpcSetConfigValues(&this->configValues);
            
            this->lastContextUpdate = armGetSystemTick();
            
    });
    
    this->listElement->addItem(this->enabledToggle);
    this->ToggleListItems[configNumber] = this->enabledToggle;
}

void GlobalOverrideGui::listUI()
{
    this->addModuleListItem(SysClkModule_CPU, &sysclk_g_freq_table_cpu_hz[0]);
    this->addModuleListItem(SysClkModule_GPU, &sysclk_g_freq_table_gpu_hz[0]);
    this->addModuleListItem(SysClkModule_MEM, &sysclk_g_freq_table_mem_hz[0]);
    //added 4 toggles
    this->addModuleListToggleListItem(3,"Uncapped GPU");
    this->addModuleListToggleListItem(4,"Fake Handheld Charg.");
    this->addModuleListToggleListItem(5,"CPU to 1785 in boost");
    this->addModuleListToggleListItem(6,"GPU to 76 in boost");
}

void GlobalOverrideGui::refresh()
{
    BaseMenuGui::refresh();

    if(this->context)
    {
        for(std::uint16_t m = 0; m < SysClkModule_EnumMax; m++)
        {
            if(this->listItems[m] != nullptr && this->listHz[m] != this->context->overrideFreqs[m])
            {
                this->listItems[m]->setValue(formatListFreqHz(this->context->overrideFreqs[m]));
                this->listHz[m] = this->context->overrideFreqs[m];
            }
        }
        
        for(std::uint16_t m = 3; m < SysClkConfigValue_EnumMax; m++)
        {
            if(this->ToggleListItems[m] != nullptr)
            {
                sysclkIpcGetConfigValues(&this->configValues);
            
                SysClkConfigValue config = (SysClkConfigValue) m;
            
                uint64_t defaultValue   = configValues.values[config];
                
                bool defValue = false;
                
                if(defaultValue==1) {
                    defValue = true;
                }
                
                this->ToggleListItems[m]->setState(defValue);
            
            }
        }
        
    }
}
