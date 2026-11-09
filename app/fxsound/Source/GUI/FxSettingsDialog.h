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
#include "FxWindow.h"
#include "FxHyperlink.h"
#include "FxController.h"
#include "FxModel.h"
#include "FxTheme.h"
#include "FxHotkeyLabel.h"
#include "FxLanguage.h"
#include "FxAudioSlider.h"
#include "FxBalanceSlider.h"
#include "FxOutputPreference.h"
#include "FxSettingRow.h"

//==============================================================================
/*
*/

class FxSettingsDialog : public FxWindow
{
public:
    FxSettingsDialog();
    ~FxSettingsDialog() = default;

	void closeButtonPressed() override;

	void showTab(int index); // 0 Audio, 1 General, 2 Soporte

	void paint(Graphics& g) override;

	bool keyPressed(const KeyPress& key) override;	

private:
	static constexpr int SEPARATOR_X = 152;

	class SettingsButton : public Button
	{
	public:
		SettingsButton(String button_name) : Button(button_name)
		{
			setMouseCursor(MouseCursor::PointingHandCursor);
		}
		~SettingsButton() = default;

		void setImage(const Drawable* image)
		{
			image_ = image->createCopy();
		}

		void paintButton(Graphics &, bool, bool) override {}
		void paint(Graphics& g) override;

	private:
		std::unique_ptr<Drawable> image_;
	};

	class SettingsPane : public Component
	{
	public:    
		SettingsPane(String name);
		~SettingsPane() = default;

	protected:
        void paint(Graphics& g) override;

		static constexpr int X_MARGIN = 20;
		static constexpr int Y_MARGIN = 5;
		static constexpr int TITLE_HEIGHT = 24;

        Label title_;

		String name_;
	};

	class AudioSettingsPane : public SettingsPane
	{
	public:
		AudioSettingsPane();
		~AudioSettingsPane();

		void resized() override;
		void paint(Graphics& g) override;

	private:
		static constexpr int GROUP_MARGIN = 10;
		static constexpr int ENDPOINT_Y = 14;
		static constexpr int LABEL_WIDTH = 220;
		static constexpr int OUTPUT_PREFERENCE_HEIGHT = 270;
		static constexpr int LABEL_HEIGHT = 14;
		static constexpr int TOGGLE_BUTTON_HEIGHT = 30;
		static constexpr int RESET_PRESETS_BUTTON_WIDTH = 220;
		static constexpr int BUTTON_HEIGHT = 24;
		static constexpr int MAX_BUTTON_WIDTH = 315;

		void setText();
		void resizeResetButton(int x, int y);

		void visibilityChanged() override;
		void mouseEnter(const MouseEvent& mouse_event) override;
		void mouseExit(const MouseEvent& mouse_event) override;

		Label output_preference_title_;
		FxOutputPreference output_preference_;
		ToggleButton prioritize_new_output_toggle_;
		Label max_boost_title_;
		TextButton max_boost_buttons_[5];

		TextButton reset_presets_button_;

		juce::Rectangle<float> output_preference_bounds_;
	};

	class GeneralSettingsPane : public SettingsPane
	{
	public:
		GeneralSettingsPane();
		~GeneralSettingsPane() = default;

		void resized() override;
		void paint(Graphics& g) override;

	private:
		static constexpr int ROW_H = 46;
		static constexpr int GAP = 8;
		static constexpr int SECTION_H = 28;

		void showLanguageMenu();
		void refreshTexts();
		void refreshShortcuts();
		bool setShortcut(int index, const KeyPress& key);

		FxSettingRow language_row_;
		FxSettingRow help_tips_row_;
		FxSettingRow notifications_row_;
		FxSettingRow startup_row_;
		FxSettingRow hotkeys_row_;
		OwnedArray<FxSettingRow> shortcut_rows_;
		FxSettingRow restore_row_;
		StringArray shortcut_cmds_;
		int section_y_[3] = { 0, 0, 0 };
		int col_x_[2] = { 0, 0 };
	};

	class HelpSettingsPane : public SettingsPane
	{
	public:
		HelpSettingsPane();
		~HelpSettingsPane() = default;

		void resized() override;
		void paint(Graphics& g) override;

	private:
		static constexpr int ROW_H = 46;
		static constexpr int GAP = 8;
		static constexpr int SECTION_H = 28;
		static constexpr int HEADER_H = 96;

		FxSettingRow mail_row_;
		FxSettingRow bug_row_;
		FxSettingRow copy_row_;
		FxSettingRow donate_row_;
		FxSettingRow license_row_;
		FxSettingRow notices_row_;
		FxSettingRow update_row_;
		FxSettingRow auto_update_row_;
		void refreshUpdateRow();
		struct Poll : Timer { std::function<void()> cb; void timerCallback() override { if (cb) cb(); } } poll_;
		int section_y_ = 0;
		int col_x_[2] = { 0, 0 };
	};

	class SettingsComponent : public Component, public Button::Listener
	{
	public:
        static constexpr int WIDTH = 720;
        static constexpr int HEIGHT = 560;

		SettingsComponent();
        ~SettingsComponent() = default;

		void resized() override;
		void paint(Graphics& g) override;

		void buttonClicked(Button* button) override;
		void selectTab(int index) { buttonClicked(index == 1 ? general_button_.get() : (index == 2 ? help_button_.get() : audio_button_.get())); }

	private:
		static constexpr int TAB_X = 20;
		static constexpr int TAB_Y = 6;
		static constexpr int TAB_WIDTH = 110;
		static constexpr int TAB_HEIGHT = 38;
		static constexpr int TAB_BAR_HEIGHT = 50;

		std::unique_ptr<SettingsButton> audio_button_;
		std::unique_ptr<SettingsButton> general_button_;
		std::unique_ptr<SettingsButton> help_button_;

		AudioSettingsPane audio_settings_pane_;
		GeneralSettingsPane general_settings_pane_;
		HelpSettingsPane help_settings_pane_;
	};

	SettingsComponent settings_content_;
	TooltipWindow tooltip_window_;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxSettingsDialog)
};
