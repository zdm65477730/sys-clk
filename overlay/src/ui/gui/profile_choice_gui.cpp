/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "profile_choice_gui.h"

#include "../format.h"

ProfileChoiceGui::ProfileChoiceGui(std::uint32_t selectedProfile, std::uint32_t* profileList, ProfileChoiceListener listener)
{
    this->selectedProfile = selectedProfile;
    this->profileList = profileList;
    this->listener = listener;
}

tsl::elm::ListItem* ProfileChoiceGui::createProfileListItem(std::uint32_t profile, bool selected)
{
	tsl::elm::ListItem* listItem = new tsl::elm::ListItem(profile ? formatListProfile(profile) : "DefaultFreqFarmatListText"_tr);
    listItem->setValue(selected ? "\uE14B" : "");

    listItem->setClickListener([this, profile](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->listener)
        {
            if(this->listener(profile))
            {
                tsl::goBack();
            }
            return true;
        }

        return false;
    });

    return listItem;
}

void ProfileChoiceGui::listUI()
{
    std::uint32_t* profilePtr = this->profileList;
    this->listElement->addItem(this->createProfileListItem(0, this->selectedProfile == 0));
    while(*profilePtr)
    {
        this->listElement->addItem(this->createProfileListItem(*profilePtr, (*profilePtr) == (this->selectedProfile)));
        profilePtr++;
    }
}
