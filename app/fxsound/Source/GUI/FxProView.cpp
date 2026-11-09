/*
FxSound
Copyright (C) 2025  FxSound LLC

Contributors:
	www.theremino.com (2025)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <JuceHeader.h>
#include "FxProView.h"
#include "FxController.h"
#include "FxTheme.h"

//==============================================================================
FxProView::FxProView() : tool_tip_(this)
{
	// The combo box still holds the preset state for the rest of the app; the chips are what the person sees.
	preset_list_.setVisible(false);
	addAndMakeVisible(preset_chips_);
	preset_chips_.onSelect = [](int index) { FxController::getInstance().setPreset(index); };

	gain_title_.setText(TRANS("Power"), NotificationType::dontSendNotification);
	gain_title_.setColour(Label::textColourId, FXCOL(DefaultText));
	gain_title_.setJustificationType(Justification::centredLeft);
	addAndMakeVisible(gain_title_);
	gain_slider_.setSliderStyle(Slider::LinearHorizontal);
	gain_slider_.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	gain_slider_.setRange(-20, FxController::getInstance().getMaxBoostDb(), 2);
	gain_slider_.setValue(FxController::getInstance().getMasterGain(), NotificationType::dontSendNotification);
	gain_slider_.onValueChange = [this]() {
		auto& controller = FxController::getInstance();
		if (controller.getMasterGain() != gain_slider_.getValue())
			controller.setMasterGain((float)gain_slider_.getValue());
		};
	addAndMakeVisible(gain_slider_);

	addAndMakeVisible(audio_controls_);
	addAndMakeVisible(equalizer_);
    addChildComponent(visualizer_);

	equalizer_.addMouseListener(this, true);
	audio_controls_.addMouseListener(this, true);

    auto& theme = dynamic_cast<LookAndFeel_V4&>(getLookAndFeel());

    tool_tip_.setColour(TooltipWindow::ColourIds::textColourId, theme.getCurrentColourScheme().getUIColour(LookAndFeel_V4::ColourScheme::defaultText));
    tool_tip_.setOpaque(false);

	setOpaque(false);
	setSize(WIDTH, HEIGHT);
}

void FxProView::startVisualizer()
{
	visualizer_.start();
}

void FxProView::pauseVisualizer()
{
	visualizer_.pause();
}

void FxProView::update()
{
	audio_controls_.update();
	equalizer_.update();
	refreshGain();

	visualizer_.calcGradient();

    if (FxController::getInstance().isAudioProcessing())
    {
        visualizer_.reset();
    }

	visualizer_.setVisible(true);
	setSize(WIDTH, HEIGHT);
	// -------------------------- Values always visible since version 2.0 (otherwise they do not appear on touch screens)
	equalizer_.showValues(true);
	audio_controls_.showValues(true);
}

void FxProView::refreshGain()
{
	auto& controller = FxController::getInstance();
	gain_slider_.setRange(-20, controller.getMaxBoostDb(), 2);
	gain_slider_.setValue(controller.getMasterGain(), NotificationType::dontSendNotification);
}

void FxPresetChips::refresh()
{
	auto& model = FxModel::getModel();
	names_.clear();
	for (int i = 0; i < model.getPresetCount(); i++)
	{
		auto preset = model.getPreset(i);
		names_.add(preset.modified ? preset.name + " *" : preset.name);
	}
	selected_ = model.getSelectedPreset();
	layoutChips(getWidth());
	repaint();
}

int FxPresetChips::heightForWidth(int width)
{
	layoutChips(width);
	return rects_.isEmpty() ? 0 : rects_.getLast().getBottom();
}

void FxPresetChips::layoutChips(int width)
{
	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
	auto font = theme.getNormalFont().withHeight(13.0f);
	rects_.clear();
	const int h = 28, gap = 5;
	int x = 0, y = 0;
	for (auto& n : names_)
	{
		int w = (int)font.getStringWidth(n) + 16;
		if (x > 0 && x + w > width) { x = 0; y += h + gap; }
		rects_.add({ x, y, w, h });
		x += w + gap;
	}
}

void FxPresetChips::resized()
{
	layoutChips(getWidth());
}

void FxPresetChips::paint(Graphics& g)
{
	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
	g.setFont(theme.getNormalFont().withHeight(13.0f));
	const bool on = isEnabled();
	for (int i = 0; i < rects_.size(); i++)
	{
		auto r = rects_[i].toFloat();
		const bool sel = (i == selected_);
		if (sel)
		{
			g.setColour(FXCOL(SliderTrack).withAlpha(on ? 0.22f : 0.08f));
			g.fillRoundedRectangle(r, r.getHeight() / 2);
			g.setColour(on ? FXCOL(SliderTrack) : FXCOL(HintText));
			g.drawRoundedRectangle(r.reduced(0.5f), r.getHeight() / 2, 1.2f);
			g.setColour(on ? FXCOL(HighlightedText) : FXCOL(HintText));
		}
		else
		{
			g.setColour(FXCOL(Outline));
			g.drawRoundedRectangle(r.reduced(0.5f), r.getHeight() / 2, 1.0f);
			g.setColour(on ? FXCOL(DefaultText) : FXCOL(HintText).withAlpha(0.6f));
		}
		g.drawText(names_[i], rects_[i], Justification::centred);
	}
}

void FxPresetChips::mouseUp(const MouseEvent& e)
{
	if (!isEnabled() || !onSelect || e.mouseWasDraggedSinceMouseDown())
		return;
	for (int i = 0; i < rects_.size(); i++)
		if (rects_[i].contains(e.getPosition()))
		{
			onSelect(i);
			return;
		}
}

void FxProView::setLookAndFeel()
{
	audio_controls_.setLookAndFeel();
}

void FxProView::resized()
{
    endpoint_list_.setBounds(PAD, 24, LIST_WIDTH, LIST_HEIGHT);

    const int chips_y = endpoint_list_.getBottom() + 16;
    const int chips_h = preset_chips_.heightForWidth(LIST_WIDTH);
    preset_chips_.setBounds(PAD, chips_y, LIST_WIDTH, chips_h);

    gain_title_.setBounds(PAD, preset_chips_.getBottom() + 16, LIST_WIDTH, 18);
    gain_slider_.setBounds(PAD, gain_title_.getBottom() + 2, LIST_WIDTH, 24);

    const int effects_y = gain_slider_.getBottom() + 14;
    audio_controls_.setBounds(PAD, effects_y, audio_controls_.getWidth(), HEIGHT - effects_y - 16);

    visualizer_.setBounds(STAGE_X, 20, STAGE_WIDTH, HEIGHT - 20 - 16 - equalizer_.getHeight() - 20);
    equalizer_.setBounds(STAGE_X, visualizer_.getBottom() + 16, STAGE_WIDTH, equalizer_.getHeight());
}

void FxProView::paint(Graphics& g)
{
	auto& theme = dynamic_cast<LookAndFeel_V4&>(getLookAndFeel());

	g.setFillType(FillType(theme.getCurrentColourScheme().getUIColour(LookAndFeel_V4::ColourScheme::windowBackground)));
	g.fillAll();

	// Side rail
	g.setColour(FXCOL(WidgetBackground));
	g.fillRect(0, 0, RAIL_WIDTH, getHeight());
	g.setColour(FXCOL(Outline));
	g.fillRect(RAIL_WIDTH, 0, 1, getHeight());

    auto enable_controls = FxModel::getModel().getPowerState();

    preset_list_.setEnabled(enable_controls);
    preset_chips_.setEnabled(enable_controls);
    gain_slider_.setEnabled(enable_controls);
    audio_controls_.setEnabled(enable_controls);
    equalizer_.setEnabled(enable_controls);
	visualizer_.setEnabled(enable_controls);
}

void FxProView::comboBoxChanged(ComboBox* combobox)
{
	FxView::comboBoxChanged(combobox);
}

void FxProView::modelChanged(FxModel::Event model_event)
{
	FxView::modelChanged(model_event);

	if (model_event == FxModel::Event::PresetSelected || model_event == FxModel::Event::PresetListUpdated || model_event == FxModel::Event::PresetModified)
	{
		preset_chips_.refresh();
		resized();
		repaint();
	}

	if (model_event == FxModel::Event::PresetSelected)
	{
		update();
	}
}

void FxProView::mouseEnter(const MouseEvent& mouse_event)
{
	FxView::mouseEnter(mouse_event);
}

void FxProView::mouseExit(const MouseEvent& mouse_event)
{
	FxView::mouseEnter(mouse_event);
}