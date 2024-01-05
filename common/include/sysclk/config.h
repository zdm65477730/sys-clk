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
    SysClkConfigValue_FreqLogIntervalMs,
    SysClkConfigValue_PowerLogIntervalMs,
    SysClkConfigValue_CsvWriteIntervalMs,
    SysClkConfigValue_UncappedGPUEnabled,
    SysClkConfigValue_OverrideMEMEnabled,
    SysClkConfigValue_FakeProfileModeEnabled,
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
        case SysClkConfigValue_FreqLogIntervalMs:
            return pretty ? "Frequency logging interval (ms)" : "freq_log_interval_ms";
        case SysClkConfigValue_PowerLogIntervalMs:
            return pretty ? "Power logging interval (ms)" : "power_log_interval_ms";
        case SysClkConfigValue_CsvWriteIntervalMs:
            return pretty ? "CSV write interval (ms)" : "csv_write_interval_ms";
        case SysClkConfigValue_UncappedGPUEnabled:
            return pretty ? "Uncapped GPU (does not change profile)" : "uncapped_gpu_enabled";
        case SysClkConfigValue_OverrideMEMEnabled:
            return pretty ? "Override MEM to MAX (1600 MHz or higher)" : "override_mem_enabled";
        case SysClkConfigValue_FakeProfileModeEnabled:
            return pretty ? "Minimum profile (spoof profile)" : "fake_profile_mode_enabled";
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
        case SysClkConfigValue_FreqLogIntervalMs:
        case SysClkConfigValue_PowerLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
        case SysClkConfigValue_UncappedGPUEnabled:
        case SysClkConfigValue_OverrideMEMEnabled:
        case SysClkConfigValue_FakeProfileModeEnabled:
            return 0ULL;
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
        case SysClkConfigValue_OverrideMEMEnabled:
            return (input == 0 || input == 1);
        case SysClkConfigValue_FakeProfileModeEnabled:
            return (input >=  0 && input < 5);
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_FreqLogIntervalMs:
        case SysClkConfigValue_PowerLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
            return input >= 0;
        default:
            return false;
    }
}
