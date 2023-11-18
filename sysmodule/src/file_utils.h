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
#include <switch.h>
#include <time.h>
#include <vector>
#include <string>
#include <cstring>
#include <atomic>
#include <cstdarg>
#include <sysclk.h>

#define FILE_CONFIG_DIR "/config/" TARGET
#define FILE_FLAG_CHECK_INTERVAL_NS 5000000000ULL
#define FILE_CONTEXT_CSV_PATH FILE_CONFIG_DIR "/context.csv"
#define FILE_LOG_FLAG_PATH FILE_CONFIG_DIR "/log.flag"
#define FILE_LOG_FILE_PATH FILE_CONFIG_DIR "/log.txt"

typedef struct CustTable {
    u8  cust[4] = {'C', 'U', 'S', 'T'};
    u32 custRev;
    u32 mtcConf;
    u32 commonCpuBoostClock;
    u32 commonEmcMemVolt;
    u32 eristaCpuMaxVolt;
    u32 eristaEmcMaxClock;
    u32 marikoCpuMaxVolt;
    u32 marikoEmcMaxClock;
    u32 marikoEmcVddqVolt;
} CustTable;

class FileUtils
{
  public:
    static void Exit();
    static Result Initialize();
    static bool IsInitialized();
    static bool IsLogEnabled();
    static void InitializeAsync();
    static void LogLine(const char *format, ...);
    static void WriteContextToCsv(const SysClkContext* context);
    static void ParseLoaderKip();
  protected:
    static void RefreshFlags(bool force);
    static Result CustParser(const char* path, size_t filesize);
};
