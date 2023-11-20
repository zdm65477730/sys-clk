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
        
        this->customListItems[m] = nullptr;
        this->customListProfiles[m] = 0;
    }
}

void GlobalOverrideGui::openFreqChoiceGui(SysClkModule module)
{
    std::uint32_t hzList[SYSCLK_FREQ_LIST_MAX];
    std::uint32_t hzCount;
    Result rc = sysclkIpcGetFreqList(module, &hzList[0], SYSCLK_FREQ_LIST_MAX, &hzCount);
    if(R_FAILED(rc))
    {
        FatalGui::openWithResultCode("sysclkIpcGetFreqList", rc);
        return;
    }

    tsl::changeTo<FreqChoiceGui>(this->context->overrideFreqs[module], hzList, hzCount, [this, module](std::uint32_t hz) {
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

void GlobalOverrideGui::openProfileChoiceGui(int configNumber, std::uint32_t* profileList)
{
    sysclkIpcGetConfigValues(&this->configValues);

    SysClkConfigValue config = (SysClkConfigValue) configNumber;
    
    tsl::changeTo<ProfileChoiceGui>((uint64_t) configValues.values[config], profileList, [this,config](std::uint32_t profile) {
        
        uint64_t uvalue = (uint64_t) profile;
        
        // Save the config
        this->configValues.values[config] = uvalue;
        
        Result rc =  sysclkIpcSetConfigValues(&this->configValues);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
            return false;
            
        }
        
        this->lastContextUpdate = armGetSystemTick();
        return true;
        
        
    });
}


void GlobalOverrideGui::addModuleListItem(SysClkModule module)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(sysclkFormatModule(module, true));
    listItem->setValue(formatListFreqMHz(0));

    listItem->setClickListener([this, module](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            this->openFreqChoiceGui(module);
            return true;
        }

        return false;
    });

    this->listElement->addItem(listItem);
    this->listItems[module] = listItem;
}

void GlobalOverrideGui::addCustomListItem(int configNumber,std::string shortLabel, std::uint32_t* profileList)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(shortLabel);
    listItem->setValue(formatListProfile(0));

    listItem->setClickListener([this, profileList,configNumber](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            this->openProfileChoiceGui(configNumber,profileList);
            return true;
        }

        return false;
    });

    this->listElement->addItem(listItem);
    this->customListItems[configNumber] = listItem;
}

void GlobalOverrideGui::addCustomToggleListItem(int configNumber,std::string shortLabel)
{
       
    sysclkIpcGetConfigValues(&this->configValues);

    SysClkConfigValue config = (SysClkConfigValue) configNumber;
    
    uint64_t defaultValue   = configValues.values[config];
    
    bool defValue = false;
    
    if(defaultValue==1) {
        defValue = true;
    }

    this->enabledToggle = new tsl::elm::ToggleListItem(shortLabel, defValue,"Yes","No");

    enabledToggle->setStateChangedListener([this,config](bool state) {
            
            int value = 0;
            
            if(state) {
                value = 1;
            }
            
            uint64_t uvalue = (uint64_t) value;
            
            // Save the config
            this->configValues.values[config] = uvalue;
        
            Result rc =  sysclkIpcSetConfigValues(&this->configValues);
            if(R_FAILED(rc))
            {
                FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
                return false;
            }
            
            this->lastContextUpdate = armGetSystemTick();
            return true;
    });
    
    this->listElement->addItem(this->enabledToggle);
    this->ToggleListItems[configNumber] = this->enabledToggle;
}

void GlobalOverrideGui::listUI()
{
    this->addModuleListItem(SysClkModule_CPU);
    this->addModuleListItem(SysClkModule_GPU);
    this->addModuleListItem(SysClkModule_MEM);
    //added 3 custom configs
    this->addCustomToggleListItem(5,"Uncapped GPU");
    this->addCustomToggleListItem(6,"Override MEM to MAX");
    this->addCustomListItem(7,"Min. profile",&sysclk_g_profile_table[0]);


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
        

        
        for(std::uint16_t m = 5; m < SysClkConfigValue_EnumMax; m++)
        {
            
            if(m == (SysClkConfigValue_EnumMax - 1)) {

                sysclkIpcGetConfigValues(&this->configValues);
            
                SysClkConfigValue config = (SysClkConfigValue) m;
            
                uint32_t defaultValue   = configValues.values[config];
                
                
                if(this->customListItems[m] != nullptr && this->customListProfiles[m] != defaultValue)
                {
                    this->customListItems[m]->setValue(formatListProfile(defaultValue));
                    this->customListProfiles[m] = defaultValue;
                }
                
            } else {
 
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
}
