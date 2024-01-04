/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "errors.h"
#include "clock_manager.h"
#include "file_utils.h"
#include "clocks.h"
#include "process_management.h"

ClockManager* ClockManager::instance = NULL;

ClockManager* ClockManager::GetInstance()
{
    return instance;
}

bool ClockManager::IsCpuBoostMode()
{
    std::uint32_t confId = 0;
    Result rc = 0;
    rc = apmExtGetCurrentPerformanceConfiguration(&confId);
    ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");
    if(confId == 0x92220009 || confId == 0x9222000A)
        return true;
    else
        return false;
}

bool ClockManager::IsGpuThrottleMode()
{
    std::uint32_t confId = 0;
    Result rc = 0;
    rc = apmExtGetCurrentPerformanceConfiguration(&confId);
    ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");
    if(confId == 0x92220009 || confId == 0x9222000A || confId == 0x9222000B || confId == 0x9222000C)
        return true;
    else
        return false;
}


void ClockManager::Exit()
{
    if(instance)
    {
        delete instance;
    }
}

void ClockManager::Initialize()
{
    if(!instance)
    {
        instance = new ClockManager();
    }
}

ClockManager::ClockManager()
{
    this->config = Config::CreateDefault();
    this->context = new SysClkContext;
    this->context->applicationId = 0;
    this->context->profile = SysClkProfile_Handheld;
    this->context->enabled = false;
    for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
    {
        this->context->freqs[i] = 0;
        this->context->overrideFreqs[i] = 0;
    }
    this->running = false;
    this->lastTempLogNs = 0;
    this->lastCsvWriteNs = 0;
}

ClockManager::~ClockManager()
{
    delete this->config;
    delete this->context;
}

void ClockManager::SetRunning(bool running)
{
    this->running = running;
}

bool ClockManager::Running()
{
    return this->running;
}

void ClockManager::Tick()
{
    std::scoped_lock lock{this->contextMutex};
    bool configRefreshed = this->config->Refresh();
    
    if (this->RefreshContext() || configRefreshed)
    {
        // reset clocks after config change, otherwise choosing do not override  for cpu/gpu/mem will not reset clocks to stock properly
        if(configRefreshed)
        {
            Clocks::ResetToStock();
        }
        std::uint32_t hz = 0;
        std::uint32_t ug = 0;
        std::uint32_t ogb = 0;
        std::uint32_t ocb = 0;
        for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            hz = this->context->overrideFreqs[module];

            if(!hz)
            {
                hz = this->config->GetAutoClockHz(this->context->applicationId, (SysClkModule)module, this->context->profile);
            }
            
            // Global default profile for applications without an application specific profile
            if(!hz)
            {
                hz = this->config->GetAutoClockHz(9999999999999999, (SysClkModule)module, this->context->profile);
            }

            if (hz)
            {
                ug = this->GetConfig()->GetConfigValue(SysClkConfigValue_UncappedGPUEnabled);
                
                if (1 == ug)
                {
                    hz = Clocks::GetNearestHz((SysClkModule)module, SysClkProfile_HandheldChargingOfficial, hz);
                }
                else
                {
                
                    hz = Clocks::GetNearestHz((SysClkModule)module, this->context->profile, hz);
                    
                }

                // BOOST MODE override: let boost mode do its job and then return back to sys/custom values (this can be toggled on and off)
                
                ogb = this->GetConfig()->GetConfigValue(SysClkConfigValue_OverrideGPUBoostEnabled);
                ocb = this->GetConfig()->GetConfigValue(SysClkConfigValue_OverrideCPUBoostEnabled);
                
                if ((( ((ocb == 0) || (!IsCpuBoostMode())) && module == 0) || ( ((ogb == 0) || (!IsGpuThrottleMode())) && module == 1) || (!(hz == 1600000000 && this->context->freqs[module] > 1600000000) && module == 2)) && hz != this->context->freqs[module] && this->context->enabled)
 
                {
                    FileUtils::LogLine("[mgr] %s clock set : %u.%u Mhz", Clocks::GetModuleName((SysClkModule)module, true), hz/1000000, hz/100000 - hz/1000000*10);
                    Clocks::SetHz((SysClkModule)module, hz);
                    this->context->freqs[module] = hz;
                }
            }
        }
    }
}

void ClockManager::WaitForNextTick()
{
    svcSleepThread(this->GetConfig()->GetConfigValue(SysClkConfigValue_PollingIntervalMs) * 1000000ULL);
}

bool ClockManager::RefreshContext()
{
    bool hasChanged = false;

    bool enabled = this->GetConfig()->Enabled();
    if(enabled != this->context->enabled)
    {
        this->context->enabled = enabled;
        FileUtils::LogLine("[mgr] " TARGET " status: %s", enabled ? "enabled" : "disabled");
        hasChanged = true;
    }

    std::uint64_t applicationId = ProcessManagement::GetCurrentApplicationId();
    if (applicationId != this->context->applicationId)
    {
        FileUtils::LogLine("[mgr] TitleID change: %016lX", applicationId);
        this->context->applicationId = applicationId;
        hasChanged = true;
    }

    SysClkProfile profile = Clocks::GetCurrentProfile();
    
    //choose profile
    std::uint32_t fp = this->GetConfig()->GetConfigValue(SysClkConfigValue_FakeProfileModeEnabled);
    
    if (0 != fp)
    {
        if(profile < (SysClkProfile)fp)
        {
            profile =  (SysClkProfile)fp;
            
        }
    }
    
    if (profile != this->context->profile)
    {
        FileUtils::LogLine("[mgr] Profile change: %s", Clocks::GetProfileName(profile, true));
        this->context->profile = profile;
        hasChanged = true;
    }

    // restore clocks to stock values on app or profile change
    if(hasChanged)
    {
        Clocks::ResetToStock();
    }

    std::uint32_t hz = 0;
    for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
    {
        hz = Clocks::GetCurrentHz((SysClkModule)module);
        if (hz != 0 && hz != this->context->freqs[module])
        {
            FileUtils::LogLine("[mgr] %s clock change: %u.%u Mhz", Clocks::GetModuleName((SysClkModule)module, true), hz/1000000, hz/100000 - hz/1000000*10);
            this->context->freqs[module] = hz;
            hasChanged = true;
        }

        hz = this->GetConfig()->GetOverrideHz((SysClkModule)module);
        
        // Override MEM to 1600
        if (module == 2)
        {
            std::uint32_t om = this->GetConfig()->GetConfigValue(SysClkConfigValue_OverrideMEMEnabled);
            
            if (1 == om)
            {
                hz = 1600000000;
            }

        }
        
        if (hz != this->context->overrideFreqs[module])
        {
            if(hz)
            {
                FileUtils::LogLine("[mgr] %s override change: %u.%u Mhz", Clocks::GetModuleName((SysClkModule)module, true), hz/1000000, hz/100000 - hz/1000000*10);
            }
            else
            {
                FileUtils::LogLine("[mgr] %s override disabled", Clocks::GetModuleName((SysClkModule)module, true));
                Clocks::ResetToStock();
                
            }
            this->context->overrideFreqs[module] = hz;
            hasChanged = true;
        }
    }

    // temperatures do not and should not force a refresh, hasChanged untouched
    std::uint32_t millis = 0;
    std::uint64_t ns = armTicksToNs(armGetSystemTick());
    std::uint64_t tempLogInterval = this->GetConfig()->GetConfigValue(SysClkConfigValue_TempLogIntervalMs) * 1000000ULL;
    bool shouldLogTemp = tempLogInterval && ((ns - this->lastTempLogNs) > tempLogInterval);
    for (unsigned int sensor = 0; sensor < SysClkThermalSensor_EnumMax; sensor++)
    {
        millis = Clocks::GetTemperatureMilli((SysClkThermalSensor)sensor);
        if(shouldLogTemp)
        {
            FileUtils::LogLine("[mgr] %s temp: %u.%u °C", Clocks::GetThermalSensorName((SysClkThermalSensor)sensor, true), millis/1000, (millis - millis/1000*1000) / 100);
        }
        this->context->temps[sensor] = millis;
    }

    if(shouldLogTemp)
    {
        this->lastTempLogNs = ns;
    }

    std::uint64_t csvWriteInterval = this->GetConfig()->GetConfigValue(SysClkConfigValue_CsvWriteIntervalMs) * 1000000ULL;

    if(csvWriteInterval && ((ns - this->lastCsvWriteNs) > csvWriteInterval))
    {
        FileUtils::WriteContextToCsv(this->context);
        this->lastCsvWriteNs = ns;
    }

    return hasChanged;
}

SysClkContext ClockManager::GetCurrentContext()
{
    std::scoped_lock lock{this->contextMutex};
    return *this->context;
}

Config* ClockManager::GetConfig()
{
    return this->config;
}
