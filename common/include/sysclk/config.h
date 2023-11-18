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

#include <stdint.h>
#include <stddef.h>

typedef enum {
    SysClkConfigValue_PollingIntervalMs = 0,
    SysClkConfigValue_TempLogIntervalMs,
    SysClkConfigValue_CsvWriteIntervalMs,
    SysClkConfigValue_UncappedGPUEnabled,
    SysClkConfigValue_FakeProfileModeEnabled,
    SysClkConfigValue_OverrideCPUBoostEnabled,
    SysClkConfigValue_OverrideGPUBoostEnabled,
    SysClkConfigValue_OverrideMEMEnabled,
    SysClkConfigValue_EnumMax,
} SysClkConfigValue;

typedef struct {
    uint64_t values[SysClkConfigValue_EnumMax];
} SysClkConfigValueList;

static inline const char* sysclkFormatConfigValue(SysClkConfigValue val, bool pretty)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return pretty ? "Polling Interval (ms)" : "poll_interval_ms";
        case SysClkConfigValue_TempLogIntervalMs:
            return pretty ? "Temperature logging interval (ms)" : "temp_log_interval_ms";
        case SysClkConfigValue_CsvWriteIntervalMs:
            return pretty ? "CSV write interval (ms)" : "csv_write_interval_ms";
        case SysClkConfigValue_UncappedGPUEnabled:
            return pretty ? "Uncapped GPU (does not change profile)" : "uncapped_gpu_enabled";
        case SysClkConfigValue_FakeProfileModeEnabled:
            return pretty ? "Minimum profile (spoof profile)" : "fake_profile_mode_enabled";
        case SysClkConfigValue_OverrideCPUBoostEnabled:
            return pretty ? "Set CPU to 1785 MHz during boost" : "override_cpu_boost_enabled";
        case SysClkConfigValue_OverrideGPUBoostEnabled:
            return pretty ? "Set GPU to 76 MHz during boost" : "override_gpu_boost_enabled";
        case SysClkConfigValue_OverrideMEMEnabled:
            return pretty ? "Override MEM to MAX (1600 MHz or higher)" : "override_mem_enabled";
        default:
            return NULL;
    }
}

static inline uint64_t sysclkDefaultConfigValue(SysClkConfigValue val)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return 300ULL;
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
        case SysClkConfigValue_UncappedGPUEnabled:
        case SysClkConfigValue_FakeProfileModeEnabled:
        case SysClkConfigValue_OverrideMEMEnabled:
            return 0ULL;
        case SysClkConfigValue_OverrideCPUBoostEnabled:
        case SysClkConfigValue_OverrideGPUBoostEnabled:
            return 1ULL;
        default:
            return 0ULL;
    }
}

static inline uint64_t sysclkValidConfigValue(SysClkConfigValue val, uint64_t input)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return input > 0;
        case SysClkConfigValue_UncappedGPUEnabled:
        case SysClkConfigValue_OverrideCPUBoostEnabled:
        case SysClkConfigValue_OverrideGPUBoostEnabled:
        case SysClkConfigValue_OverrideMEMEnabled:
            return (input == 0 || input == 1);
        case SysClkConfigValue_FakeProfileModeEnabled:
            return (input >=  0 && input < 5);
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
            return true;
        default:
            return false;
    }
}
