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

#include "cheat_sheet_tab.h"

using namespace brls::i18n::literals;

CheatSheetTab::CheatSheetTab()
{
    // CPU
    this->addView(new brls::Header("application/manager/mainframe/cheatSheetTab/cpuHeader"_i18n));
    brls::Table *cpuTable = new brls::Table();

    cpuTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/cpuMaxTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/cpuMaxValueTableRow"_i18n);
    cpuTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/cpuOfficialDockedAndHandheldTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/cpuOfficialDockedAndHandheldValueTableRow"_i18n);

    this->addView(cpuTable);

    // GPU
    this->addView(new brls::Header("application/manager/mainframe/cheatSheetTab/gpuHeader"_i18n));
    brls::Table *gpuTable = new brls::Table();

    gpuTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/gpuMaxTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/gpuMaxValueTableRow"_i18n);
    gpuTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/gpuOfficialDockedTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/gpuOfficialDockedValueTableRow"_i18n);
    gpuTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/gpuMaximumHandheldTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/gpuMaximumHandheldValueTableRow"_i18n);
    gpuTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/gpuOfficialHandheldTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/gpuOfficialHandheldValueTableRow"_i18n);

    this->addView(gpuTable);

    // MEM
    this->addView(new brls::Header("application/manager/mainframe/cheatSheetTab/memHeader"_i18n));
    brls::Table *memTable = new brls::Table();

    memTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/memMaxOfficialDockedTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/memMaxOfficialDockedValueTableRow"_i18n);
    memTable->addRow(brls::TableRowType::BODY, "application/manager/mainframe/cheatSheetTab/memOfficialHandheldTableRow"_i18n, "application/manager/mainframe/cheatSheetTab/memOfficialHandheldValueTableRow"_i18n);

    this->addView(memTable);
}

void CheatSheetTab::customSpacing(brls::View* current, brls::View* next, int* spacing)
{
    if (dynamic_cast<brls::Table*>(current))
        *spacing = 0;
    else
        List::customSpacing(current, next, spacing);
}
