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

#include "FxPowerButton.h"
#include "FxTheme.h"

FxPowerButton::FxPowerButton(const String &button_name) : DrawableButton(button_name, DrawableButton::ButtonStyle::ImageFitted)
{
	power_state_ = false;
	knob_ = 0.0f;

	power_on_image_ = Drawable::createFromImageData(FXIMAGE(PowerOnButton), FXIMAGESIZE(PowerOnButton));
	power_off_image_ = Drawable::createFromImageData(FXIMAGE(PowerOffButton), FXIMAGESIZE(PowerOffButton));
}

void FxPowerButton::paint(Graphics& g)
{
	// iOS-style switch: a pill with a round knob that slides.
	auto pill = getLocalBounds().toFloat().withSizeKeepingCentre(40.0f, 22.0f);
	const float r = pill.getHeight() / 2.0f;
	const bool enabled = isEnabled();
	const float k = knob_;

	const Colour on_col = FXCOL(SliderTrack);
	const Colour off_col = FXCOL(MenuBackground);
	Colour track = off_col.interpolatedWith(on_col, k);
	if (!enabled)
		track = track.withMultipliedSaturation(0.2f).withAlpha(0.5f);

	// soft glow when on
	if (k > 0.01f && enabled)
	{
		g.setColour(on_col.withAlpha(0.22f * k));
		g.fillRoundedRectangle(pill.expanded(3.0f), r + 3.0f);
	}

	g.setColour(track);
	g.fillRoundedRectangle(pill, r);
	g.setColour(FXCOL(Outline).interpolatedWith(on_col.brighter(0.2f), k));
	g.drawRoundedRectangle(pill.reduced(0.5f), r, 1.0f);

	// knob
	const float margin = 2.5f;
	const float d = pill.getHeight() - margin * 2;
	const float x = pill.getX() + margin + k * (pill.getWidth() - margin * 2 - d);
	auto knob = juce::Rectangle<float>(x, pill.getY() + margin, d, d);
	g.setColour(Colours::black.withAlpha(0.35f));
	g.fillEllipse(knob.translated(0.0f, 1.2f));
	g.setColour(enabled ? Colour(0xffeaf2ff) : Colour(0xff8a94a6));
	g.fillEllipse(knob);
}

void FxPowerButton::timerCallback()
{
	const float target = power_state_ ? 1.0f : 0.0f;
	const float step = 0.2f;
	if (std::abs(target - knob_) <= step)
	{
		knob_ = target;
		stopTimer();
	}
	else
		knob_ += (target > knob_) ? step : -step;
	repaint();
}

bool FxPowerButton::keyPressed(const KeyPress& key)
{
	if (isEnabled() && key.isKeyCode(KeyPress::spaceKey))
	{
		triggerClick();
		return true;
	}

	return false;
}

void FxPowerButton::lookAndFeelChanged()
{
	power_on_image_ = Drawable::createFromImageData(FXIMAGE(PowerOnButton), FXIMAGESIZE(PowerOnButton));
	power_off_image_ = Drawable::createFromImageData(FXIMAGE(PowerOffButton), FXIMAGESIZE(PowerOffButton));
	repaint();
}

bool FxPowerButton::getPowerState()
{
	return power_state_;
}

void FxPowerButton::setPowerState(bool power_state)
{
	power_state_ = power_state;
	startTimerHz(60);
	repaint();
}

int FxPowerButton::getImageWidth()
{
	return image_width_;
}

void FxPowerButton::setImageWidth(int image_width)
{
	image_width_ = image_width;
}