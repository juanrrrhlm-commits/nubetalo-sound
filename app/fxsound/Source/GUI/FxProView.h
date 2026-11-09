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
#include "FxAudioControls.h"
#include "FxEqualizer.h"
#include "FxVisualizer.h"

//==============================================================================
/*
*/
// Presets as a wrapping row of pills instead of a drop-down list.
class FxPresetChips : public Component
{
public:
	std::function<void(int)> onSelect;

	void refresh();
	int heightForWidth(int width);

private:
	void paint(Graphics& g) override;
	void resized() override;
	void mouseUp(const MouseEvent& e) override;
	void layoutChips(int width);

	StringArray names_;
	Array<juce::Rectangle<int>> rects_;
	int selected_ = -1;
};

class FxProView  : public FxView
{
public:
    FxProView();
    ~FxProView() = default;

	void startVisualizer();
	void pauseVisualizer();

	void update();

    void setLookAndFeel();
    void refreshGain();

private:
	// Side rail (output, preset, effects) + stage (spectrum, equalizer).
	static constexpr int WIDTH = 900;
	static constexpr int HEIGHT = 552;
	static constexpr int RAIL_WIDTH = 264;
	static constexpr int PAD = 24;
	static constexpr int LIST_WIDTH = 216;
	static constexpr int LIST_HEIGHT = 40;
	static constexpr int STAGE_X = RAIL_WIDTH + 20;
	static constexpr int STAGE_WIDTH = WIDTH - STAGE_X - 20;

	void resized() override;
	void paint(Graphics& g) override;
	void comboBoxChanged(ComboBox* combobox) override;
	void modelChanged(FxModel::Event model_event) override;

	void mouseEnter(const MouseEvent& mouse_event) override;
	void mouseExit(const MouseEvent& mouse_event) override;

	FxPresetChips preset_chips_;
	Label gain_title_;
	FxAudioSlider gain_slider_ { "%+0.0f dB", 0.0f };
	FxAudioControls audio_controls_;
	FxEqualizer equalizer_;
    TooltipWindow tool_tip_;
    FxVisualizer visualizer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxProView)
};
