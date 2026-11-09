/*
FxSound
Copyright (C) 2025  FxSound LLC

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
#include "FxLiteView.h"
#include "FxController.h"
#include "FxTheme.h"

//==============================================================================
FxLiteView::FxLiteView()
{
	// The combo box keeps the preset state for the rest of the app; the stepper is what the person sees.
	preset_list_.setVisible(false);
	addAndMakeVisible(stepper_);
	stepper_.onSelect = [](int index) { FxController::getInstance().setPreset(index); };

	gain_title_.setText(TRANS("Power"), NotificationType::dontSendNotification);
	gain_title_.setColour(Label::textColourId, FXCOL(DefaultText));
	gain_title_.setJustificationType(Justification::centredLeft);
	addAndMakeVisible(gain_title_);
	gain_slider_.setSliderStyle(Slider::LinearHorizontal);
	gain_slider_.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	refreshGain();
	gain_slider_.onValueChange = [this]() {
		auto& controller = FxController::getInstance();
		if (controller.getMasterGain() != gain_slider_.getValue())
			controller.setMasterGain((float)gain_slider_.getValue());
		};
	addAndMakeVisible(gain_slider_);

	setOpaque(false);
	setSize(WIDTH, HEIGHT);
}

void FxLiteView::refreshGain()
{
	auto& controller = FxController::getInstance();
	gain_slider_.setRange(-20, controller.getMaxBoostDb(), 2);
	gain_slider_.setValue(controller.getMasterGain(), NotificationType::dontSendNotification);
}

void FxLiteView::modelChanged(FxModel::Event model_event)
{
	FxView::modelChanged(model_event);
	if (model_event == FxModel::Event::PresetSelected || model_event == FxModel::Event::PresetListUpdated || model_event == FxModel::Event::PresetModified)
	{
		stepper_.refresh();
		refreshGain();
	}
}

void FxLiteView::resized()
{
	const int w = getWidth() - PAD * 2;
	endpoint_list_.setBounds(PAD, PAD, w, ROW_H);

	const int y2 = endpoint_list_.getBottom() + 12;
	stepper_.setBounds(PAD, y2, 240, ROW_H);
	gain_title_.setBounds(stepper_.getRight() + 20, y2, 60, ROW_H);
	gain_slider_.setBounds(gain_title_.getRight(), y2 + 8, getWidth() - PAD - gain_title_.getRight(), 24);
}

void FxLiteView::paint(Graphics& g)
{
	auto& theme = dynamic_cast<LookAndFeel_V4&>(getLookAndFeel());

	g.setFillType(FillType(theme.getCurrentColourScheme().getUIColour(LookAndFeel_V4::ColourScheme::windowBackground)));
	g.fillAll();

	auto power_state = FxModel::getModel().getPowerState();
	stepper_.setEnabled(power_state);
	gain_slider_.setEnabled(power_state);
}

void FxPresetStepper::refresh()
{
	auto& model = FxModel::getModel();
	count_ = model.getPresetCount();
	selected_ = model.getSelectedPreset();
	if (selected_ >= 0 && selected_ < count_)
	{
		auto preset = model.getPreset(selected_);
		name_ = preset.modified ? preset.name + " *" : preset.name;
	}
	repaint();
}

void FxPresetStepper::paint(Graphics& g)
{
	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
	auto b = getLocalBounds().toFloat();
	const bool on = isEnabled();

	g.setColour(FXCOL(WidgetBackground));
	g.fillRoundedRectangle(b, 8.0f);
	g.setColour(FXCOL(Outline).brighter(0.15f));
	g.drawRoundedRectangle(b.reduced(0.5f), 8.0f, 1.0f);

	auto chevron = [&](float cx, bool left) {
		Path p;
		const float cy = b.getCentreY();
		const float d = left ? -1.0f : 1.0f;
		p.startNewSubPath(cx - 2.5f * d, cy - 5.0f);
		p.lineTo(cx + 2.5f * d, cy);
		p.lineTo(cx - 2.5f * d, cy + 5.0f);
		g.setColour(on ? FXCOL(DefaultText) : FXCOL(HintText).withAlpha(0.5f));
		g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::curved, PathStrokeType::rounded));
	};
	chevron(b.getX() + 20.0f, true);
	chevron(b.getRight() - 20.0f, false);

	g.setColour(on ? FXCOL(HighlightedText) : FXCOL(HintText));
	g.setFont(theme.getNormalFont().withHeight(15.0f));
	g.drawText(name_, getLocalBounds().reduced(40, 0), Justification::centred);
}

void FxPresetStepper::mouseUp(const MouseEvent& e)
{
	if (!isEnabled() || !onSelect || count_ <= 0 || e.mouseWasDraggedSinceMouseDown())
		return;
	const int x = e.getPosition().x;
	if (x < getWidth() / 3)
		onSelect((selected_ - 1 + count_) % count_);
	else if (x > getWidth() * 2 / 3)
		onSelect((selected_ + 1) % count_);
}
