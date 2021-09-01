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

#include <list>

#include "base_menu_gui.h"

using ProfileChoiceListener = std::function<bool(std::uint32_t profile)>;

#define FREQ_DEFAULT_TEXT "Do not override"

class ProfileChoiceGui : public BaseMenuGui
{
    protected:
        std::uint32_t selectedProfile;
        std::uint32_t* profileList;
        ProfileChoiceListener listener;
        tsl::elm::ListItem* createProfileListItem(std::uint32_t profile, bool selected);

    public:
        ProfileChoiceGui(std::uint32_t selectedProfile, std::uint32_t* profileList, ProfileChoiceListener listener);
        ~ProfileChoiceGui() {}
        void listUI() override;
};
