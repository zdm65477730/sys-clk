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

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "main_frame.h"
#include "logo.h"

#include "ipc/client.h"
#include "ipc/ipc.h"

int main(int argc, char* argv[])
{
    brls::i18n::loadTranslations();
    // Init the app
    if (!brls::Application::init(APP_TITLE))
    {
        brls::Logger::error("application/manager/main/initError"_i18n);
        return EXIT_FAILURE;
    }

    // Setup verbose logging on PC
#ifndef __SWITCH__
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
#endif

    if (brls::Application::loadFont(LOGO_FONT_NAME, LOGO_FONT_PATH) < 0)
    {
        brls::Logger::error("application/manager/main/loadFontError"_i18n);
    }

    uint32_t apiVersion;

    // Check that sys-clk is running
    if (!sysclkIpcRunning())
    {
        brls::Logger::error("application/manager/main/sysclkIpcNotRunningError"_i18n);
        brls::Application::crash("application/manager/main/sysclkIpcNotRunningCrash"_i18n);
    }
    // Initialize sys-clk IPC client
    else if (R_FAILED(sysclkIpcInitialize()) || R_FAILED(sysclkIpcGetAPIVersion(&apiVersion)))
    {
        brls::Logger::error("application/manager/main/sysclkIpcInitError"_i18n);
        brls::Application::crash("application/manager/main/sysclkIpcInitCrash"_i18n);
    }
    else if (SYSCLK_IPC_API_VERSION != apiVersion) {
        brls::Logger::error("application/manager/main/sysclkIpcVersionMismatchError"_i18n, SYSCLK_IPC_API_VERSION, apiVersion);
        brls::Application::crash("application/manager/main/sysclkIpcVersionMismatchCrash"_i18n);
    }
    else
    {
        // Set version string
        char version[0x100] = {0};
        Result rc = sysclkIpcGetVersionString(version, sizeof(version));
        if (R_SUCCEEDED(rc))
        {
            brls::Application::setCommonFooter(std::string(version));
        }
        else
        {
            brls::Logger::error("application/manager/main/sysclkIpcVersionGetError"_i18n);
            brls::Application::setCommonFooter("application/manager/main/sysclkIpcVersionGetUnknown"_i18n);
        }

        // Initialize services with a PC shim
        nsInitialize();

        // Create root view
        MainFrame *mainFrame = new MainFrame();

        // Add the root view to the stack
        brls::Application::pushView(mainFrame);
    }

    // Run the app
    while (brls::Application::mainLoop());

    // Exit
    nsExit();
    sysclkIpcExit();
    return EXIT_SUCCESS;
}
