/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019-2020  natinusala
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

#include "about_tab.h"

#include "logo.h"
#include "ipc/ipc.h"
#include "ipc/client.h"

#include <borealis.hpp>

AboutTab::AboutTab()
{
    this->addView(new Logo(LogoStyle::ABOUT));

    // Subtitle
    brls::Label *subTitle = new brls::Label(
        brls::LabelStyle::REGULAR,
        "application/manager/mainframe/aboutTab/subtitleLabel"_i18n,
        true
    );
    subTitle->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(subTitle);

    // Copyright
    brls::Label *copyright = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "application/manager/mainframe/aboutTab/copyrightLabel"_i18n,
        true
    );
    copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(copyright);

    // Links
    this->addView(new brls::Header("application/manager/mainframe/aboutTab/LinksAndResourcesHeader"_i18n));
    brls::Label *links = new brls::Label(
        brls::LabelStyle::SMALL,
        "application/manager/mainframe/aboutTab/linksLabel"_i18n,
        true
    );
    this->addView(links);
}
