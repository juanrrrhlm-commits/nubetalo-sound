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

#pragma once

#include <JuceHeader.h>
#include "FxView.h"
#include "FxAudioSlider.h"

//==============================================================================
// Preset picker for the compact view: previous / next arrows around the preset name.
class FxPresetStepper : public Component
{
public:
	std::function<void(int)> onSelect;
	void refresh();

private:
	void paint(Graphics& g) override;
	void mouseUp(const MouseEvent& e) override;

	String name_;
	int selected_ = -1;
	int count_ = 0;
};

class FxLiteView : public FxView
{
public:
    FxLiteView();
    ~FxLiteView() = default;

    void resized() override;
	void paint(Graphics& g) override;
	void refreshGain();

private:
	static constexpr int PAD = 24;
	static constexpr int ROW_H = 40;
	static constexpr int WIDTH = 520;
	static constexpr int HEIGHT = PAD + ROW_H + 12 + ROW_H + PAD;

	void modelChanged(FxModel::Event model_event) override;

	FxPresetStepper stepper_;
	Label gain_title_;
	FxAudioSlider gain_slider_ { "%+0.0f dB", 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxLiteView)
};
