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

#include "utils.h"

std::string formatFreq(uint32_t freq)
{
    char str[16];
    snprintf(str, sizeof(str), "%.1f MHz", (float)freq / 1000000.0f);
    return std::string(str);
}

std::string formatTid(uint64_t tid)
{
    char str[17];
    snprintf(str, sizeof(str), "%016lX", tid);
    return std::string(str);
}

std::string formatProfile(SysClkProfile profile, bool pretty)
{
    switch(profile)
    {
        case SysClkProfile_Docked:
            return pretty ? "application/manager/utils/formatProfileDockedPretty"_i18n : "application/manager/utils/formatProfileDocked"_i18n;
        case SysClkProfile_Handheld:
            return pretty ? "application/manager/utils/formatProfileHandheldPretty"_i18n : "application/manager/utils/formatProfileHandheld"_i18n;
        case SysClkProfile_HandheldCharging:
            return pretty ? "application/manager/utils/formatProfileHandheldChargingPretty"_i18n : "application/manager/utils/formatProfileHandheldCharging"_i18n;
        case SysClkProfile_HandheldChargingUSB:
            return pretty ? "application/manager/utils/formatProfileHandheldChargingUSBPretty"_i18n : "application/manager/utils/formatProfileHandheldChargingUSB"_i18n;
        case SysClkProfile_HandheldChargingOfficial:
            return pretty ? "application/manager/utils/formatProfileHandheldChargingOfficialPretty"_i18n : "application/manager/utils/formatProfileHandheldChargingOfficial"_i18n;
        default:
            return "";
    }
}

std::string formatTemp(uint32_t temp)
{
    char str[16];
    snprintf(str, sizeof(str), "%.1f °C", (float)temp / 1000.0f);
    return std::string(str);
}

std::string SysClkConfigValueToString(SysClkConfigValue val, bool pretty)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return pretty ? "application/manager/utils/sysclkFormatConfigValuePollingIntervalPretty"_i18n : "application/manager/utils/sysclkFormatConfigValuePollingInterval"_i18n;
        case SysClkConfigValue_TempLogIntervalMs:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueTemperatureLoggingIntervalPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueTemperatureLoggingInterval"_i18n;
        case SysClkConfigValue_CsvWriteIntervalMs:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueCSVWriteIntervalPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueCSVWriteInterval"_i18n;
        case SysClkConfigValue_UncappedGPUEnabled:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueGPUNoLimitedPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueGPUNoLimited"_i18n;
        case SysClkConfigValue_FakeProfileModeEnabled:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueLowestFakeConfigPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueLowestFakeConfig"_i18n;
        case SysClkConfigValue_OverrideCPUBoostEnabled:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueOverrideCPUBoostPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueOverrideCPUBoost"_i18n;
        case SysClkConfigValue_OverrideGPUBoostEnabled:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueOverrideGPUBoostPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueOverrideGPUBoost"_i18n;
        case SysClkConfigValue_OverrideMEMEnabled:
            return pretty ? "application/manager/utils/sysclkFormatConfigValueOverrideMEMPretty"_i18n : "application/manager/utils/sysclkFormatConfigValueOverrideMEM"_i18n;
        default:
            return "";
    }
}

void errorResult(std::string tag, Result rc)
{
#ifdef __SWITCH__
    brls::Logger::error("application/manager/utils/errorResultSwitch"_i18n, rc, tag.c_str(), R_MODULE(rc), R_DESCRIPTION(rc));
#else
    brls::Logger::error("application/manager/utils/errorResultNoneSwitch"_i18n, rc, tag.c_str());
#endif
}

// TODO: Merge ticker for single line labels in Borealis and remove usage of this
std::string formatListItemTitle(const std::string str, size_t maxScore) {
    size_t score = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        score += std::isupper(str[i]) ? 4 : 3;
        if(score > maxScore)
        {
            return str.substr(0, i-1) + "\u2026";
        }
    }

    return str;
}

brls::SelectListItem* createFreqListItem(SysClkModule module, uint32_t selectedFreqInMhz, std::string defaultString)
{
    std::string name;
    uint32_t* table;

    switch (module)
    {
        case SysClkModule_CPU:
            name = "application/manager/utils/createFreqListItem/moduleCPU"_i18n;
            table = sysclk_g_freq_table_cpu_hz;
            break;
        case SysClkModule_GPU:
            name = "application/manager/utils/createFreqListItem/moduleGPU"_i18n;
            table = sysclk_g_freq_table_gpu_hz;
            break;
        case SysClkModule_MEM:
            name = "application/manager/utils/createFreqListItem/moduleMEM"_i18n;
            table = sysclk_g_freq_table_mem_hz;
            break;
        default:
            return nullptr;
    }

    size_t selected = 0;
    size_t i        = 0;

    std::vector<std::string> clocks;

    clocks.push_back(defaultString);

    while (table[i] != 0)
    {
        uint32_t freq = table[i];

        if (freq / 1000000 == selectedFreqInMhz)
            selected = i + 1;

        char clock[16];
        snprintf(clock, sizeof(clock), "%d MHz", freq / 1000000);

        clocks.push_back(std::string(clock));

        i++;
    }

    return new brls::SelectListItem(name, clocks, selected);
}

brls::SelectListItem* createProfileListItem(std::string name, uint32_t selectedProfile, std::string defaultString)
{
    uint32_t* table  = sysclk_g_profile_table;
    
    size_t selected = 0;
    size_t i        = 0;

    std::vector<std::string> profiles;

    profiles.push_back(defaultString);

    while (table[i] != 0)
    {
        uint32_t profile = table[i];

        if (profile == selectedProfile)
            selected = profile;
        
        profiles.push_back(formatProfile((SysClkProfile)profile));
        
        i++;
    }

    return new brls::SelectListItem(name, profiles, selected);
}
