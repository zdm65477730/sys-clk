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

#include "status_tab.h"

#include "ipc/client.h"

#include "utils.h"

#define DANGEROUS_TEMP_THRESHOLD    84000
#define DANGEROUS_TEMP_COLOR        nvgRGB(255, 85, 0)

StatusTab::StatusTab(RefreshTask *refreshTask) :
    brls::List(),
    refreshTask(refreshTask)
{
    // Get context
    SysClkContext context;
    Result rc = sysclkIpcGetCurrentContext(&context);

    if (R_FAILED(rc))
    {
        brls::Logger::error("application/manager/mainframe/statusTab/sysclkIpcGetCurrentContextError"_i18n);
        errorResult("application/manager/mainframe/statusTab/sysclkIpcGetCurrentContextErrorResult"_i18n, rc);
        brls::Application::crash("application/manager/mainframe/statusTab/sysclkIpcGetCurrentContextCrash"_i18n);
        return;
    }

    // Customize list
    this->setSpacing(this->getSpacing() / 2);
    this->setMarginBottom(20);

    // Enabled option
    brls::ToggleListItem *serviceEnabledListItem = new brls::ToggleListItem("application/manager/mainframe/statusTab/serviceEnabledListItem"_i18n, context.enabled, "", "application/manager/mainframe/statusTab/serviceEnabledListItemText"_i18n, "application/manager/mainframe/statusTab/serviceDisenabledListItemText"_i18n);

    serviceEnabledListItem->getClickEvent()->subscribe([this, serviceEnabledListItem](View* view)
    {
        bool enabled = serviceEnabledListItem->getToggleState();
        brls::Logger::info("application/manager/mainframe/statusTab/newServiceStateInfo"_i18n, enabled);

        Result rc = sysclkIpcSetEnabled(enabled);

        if (R_FAILED(rc))
        {
            brls::Logger::error("application/manager/mainframe/statusTab/sysclkIpcSetEnabledError"_i18n);
            errorResult("application/manager/mainframe/statusTab/sysclkIpcSetEnabledErrorResult"_i18n, rc);
            // TODO: Put it back to on / off
        }

        this->refreshTask->fireNow();
    });

    this->addView(serviceEnabledListItem);

    // Frequencies
    brls::Header *hardwareHeader = new brls::Header("application/manager/mainframe/statusTab/hardwareHeader"_i18n);
    this->addView(hardwareHeader);

    StatusGrid *frequenciesLayout = new StatusGrid();
    frequenciesLayout->setSpacing(22);
    frequenciesLayout->setHeight(40);

    this->cpuFreqCell = new StatusCell("application/manager/mainframe/statusTab/cpuStatusCell"_i18n, formatFreq(context.freqs[SysClkModule_CPU]));
    this->gpuFreqCell = new StatusCell("application/manager/mainframe/statusTab/gpuStatusCell"_i18n, formatFreq(context.freqs[SysClkModule_GPU]));
    this->memFreqCell = new StatusCell("application/manager/mainframe/statusTab/memStatusCell"_i18n, formatFreq(context.freqs[SysClkModule_MEM]));

    frequenciesLayout->addView(this->cpuFreqCell);
    frequenciesLayout->addView(this->gpuFreqCell);
    frequenciesLayout->addView(this->memFreqCell);

    this->addView(frequenciesLayout);

    // Temperatures
    StatusGrid *tempsLayout = new StatusGrid();
    tempsLayout->setSpacing(22);
    tempsLayout->setHeight(40);

    this->skinTempCell = new StatusCell("application/manager/mainframe/statusTab/skinStatusCell"_i18n, formatTemp(context.temps[SysClkThermalSensor_Skin]));
    this->socTempCell = new StatusCell("application/manager/mainframe/statusTab/socStatusCell"_i18n, formatTemp(context.temps[SysClkThermalSensor_SOC]));
    this->pcbTempCell = new StatusCell("application/manager/mainframe/statusTab/pcbStatusCell"_i18n, formatTemp(context.temps[SysClkThermalSensor_PCB]));

    if (context.temps[SysClkThermalSensor_SOC] > DANGEROUS_TEMP_THRESHOLD)
        this->socTempCell->setValueColor(DANGEROUS_TEMP_COLOR);

    if (context.temps[SysClkThermalSensor_PCB] > DANGEROUS_TEMP_THRESHOLD)
        this->pcbTempCell->setValueColor(DANGEROUS_TEMP_COLOR);

    if (context.temps[SysClkThermalSensor_Skin] > DANGEROUS_TEMP_THRESHOLD)
        this->skinTempCell->setValueColor(DANGEROUS_TEMP_COLOR);

    tempsLayout->addView(this->socTempCell);
    tempsLayout->addView(this->pcbTempCell);
    tempsLayout->addView(this->skinTempCell);

    this->addView(tempsLayout);

    // Info
    brls::Header *systemHeader = new brls::Header("application/manager/mainframe/statusTab/systemHeader"_i18n);
    this->addView(systemHeader);

    InfoGrid *infoLayout = new InfoGrid();
    infoLayout->setSpacing(22);
    infoLayout->setHeight(40);

    this->profileCell   = new StatusCell("application/manager/mainframe/statusTab/profileStatusCell"_i18n, formatProfile(context.profile));
    this->tidCell       = new StatusCell("application/manager/mainframe/statusTab/tidStatusCell"_i18n, formatTid(context.applicationId));

    infoLayout->addView(this->profileCell);
    infoLayout->addView(this->tidCell);

    this->addView(infoLayout);

    // Warning label
    warningLabel = new brls::Label(brls::LabelStyle::SMALL, "");

    warningLabel->setVerticalAlign(NVG_ALIGN_BOTTOM);
    warningLabel->setHorizontalAlign(NVG_ALIGN_RIGHT);

    this->addView(warningLabel, true);

    updateWarningForProfile(context.profile, false);

    // Setup refresh task
    this->freqListenerSub = refreshTask->registerFreqListener([this](SysClkModule module, uint32_t freq) {
        switch(module)
        {
            case SysClkModule_CPU:
                this->cpuFreqCell->setValue(formatFreq(freq));
                break;
            case SysClkModule_GPU:
                this->gpuFreqCell->setValue(formatFreq(freq));
                break;
            case SysClkModule_MEM:
                this->memFreqCell->setValue(formatFreq(freq));
                break;
            default:
                break;
        }
    });

    this->appIdListenerSub = refreshTask->registerAppIdListener([this](uint64_t tid) {
        this->tidCell->setValue(formatTid(tid));
    });

    this->profileListenerSub = refreshTask->registerProfileListener([this](SysClkProfile profile) {
        this->profileCell->setValue(formatProfile(profile));
        updateWarningForProfile(profile, true);
    });

    this->tempListenerSub = refreshTask->registerTempListener([this](SysClkThermalSensor sensor, uint32_t temp) {
        switch (sensor)
        {
            case SysClkThermalSensor_PCB:
                this->pcbTempCell->setValue(formatTemp(temp));

                if (temp > DANGEROUS_TEMP_THRESHOLD)
                    this->pcbTempCell->setValueColor(DANGEROUS_TEMP_COLOR);
                else
                    this->pcbTempCell->resetValueColor();
                break;
            case SysClkThermalSensor_SOC:
                this->socTempCell->setValue(formatTemp(temp));

                if (temp > DANGEROUS_TEMP_THRESHOLD)
                    this->socTempCell->setValueColor(DANGEROUS_TEMP_COLOR);
                else
                    this->socTempCell->resetValueColor();

                break;
            case SysClkThermalSensor_Skin:
                this->skinTempCell->setValue(formatTemp(temp));

                if (temp > DANGEROUS_TEMP_THRESHOLD)
                    this->skinTempCell->setValueColor(DANGEROUS_TEMP_COLOR);
                else
                    this->skinTempCell->resetValueColor();
                break;
            default:
                break;
        }
    });
}

void StatusTab::updateWarningForProfile(SysClkProfile profile, bool animated)
{
    switch (profile)
    {
        case SysClkProfile_Handheld:
            if (this->warningLabel->isHidden())
                this->warningLabel->show([](){});

            this->warningLabel->setText("application/manager/mainframe/statusTab/updateWarningForProfileHandheldLabel"_i18n + formatFreq(SYSCLK_GPU_HANDHELD_MAX_HZ) + "application/manager/mainframe/statusTab/updateWarningResultForProfileHandheldLabel"_i18n);
            break;
        case SysClkProfile_HandheldChargingUSB:
            if (this->warningLabel->isHidden())
                this->warningLabel->show([](){});

            this->warningLabel->setText("application/manager/mainframe/statusTab/updateWarningForProfileHandheldChargingUSBLabel"_i18n + formatFreq(SYSCLK_GPU_UNOFFICIAL_CHARGER_MAX_HZ) + "application/manager/mainframe/statusTab/updateWarningResultForProfileHandheldChargingUSBLabel"_i18n);
            break;
        default:
            if (!this->warningLabel->isHidden())
                this->warningLabel->hide([](){}, animated);
            break;
    }
}

StatusTab::~StatusTab()
{
    refreshTask->unregisterFreqListener(this->freqListenerSub);
    refreshTask->unregisterAppIdListener(this->appIdListenerSub);
    refreshTask->unregisterProfileListener(this->profileListenerSub);
    refreshTask->unregisterTempListener(this->tempListenerSub);
}

StatusGrid::StatusGrid() 
    : BoxLayout(brls::BoxLayoutOrientation::HORIZONTAL)
{

}

void StatusGrid::layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash)
{
    // Distribute space equally between children
    if (!this->children.empty())
    {
        size_t size     = this->children.size();
        unsigned width  = (this->getWidth()- this->marginLeft - this->marginRight - this->getSpacing() * (size - 1)) / size;

        for (brls::BoxLayoutChild *child : this->children)
        {
            if (!child->fill)
                child->view->setWidth(width);
        }
    }

    // Layout normally
    BoxLayout::layout(vg, style, stash);
}

InfoGrid::InfoGrid() : BoxLayout(brls::BoxLayoutOrientation::HORIZONTAL)
{

}

void InfoGrid::layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash)
{
    // Give first child one third of the view, give second child the rest
    if (this->children.size() >= 2)
    {
        size_t size             = 3;
        unsigned firstWidth     = (this->getWidth()- this->marginLeft - this->marginRight - this->getSpacing() * (size - 1)) / size;
        unsigned secondWidth    = firstWidth * 2 + this->getSpacing();

        this->children[0]->view->setWidth(firstWidth);
        this->children[1]->view->setWidth(secondWidth);
    }

    // Layout normally
    BoxLayout::layout(vg, style, stash);
}

StatusCell::StatusCell(std::string label, std::string value) : label(label), value(value)
{
    this->resetValueColor();
}

void StatusCell::setValue(std::string value)
{
    this->value = value;
}

void StatusCell::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx)
{
    unsigned padding = 5;

    // Label
    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->tableBodyTextColor));
    nvgFontSize(vg, 16);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
    nvgText(vg, x + padding, y + height / 2, this->label.c_str(), nullptr);

    // Value
    nvgBeginPath(vg);
    nvgFillColor(vg, a(this->valueColor));
    nvgFontSize(vg, 20);
    nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE);
    nvgText(vg, x + width - padding, y + height / 2, this->value.c_str(), nullptr);
}

void StatusCell::setValueColor(NVGcolor color)
{
    this->valueColor = color;
}

void StatusCell::resetValueColor()
{
    this->valueColor = brls::Application::getTheme()->textColor;
}
