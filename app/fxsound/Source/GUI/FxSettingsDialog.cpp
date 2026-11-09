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
#include "FxSettingsDialog.h"
#include "FxDonate.h"
#include "FxSupport.h"
#include "../Utils/SysInfo/SysInfo.h"

//==============================================================================
FxSettingsDialog::FxSettingsDialog() : FxWindow("Settings"), tooltip_window_(this)
{
	setContent(&settings_content_);
	centreWithSize(getWidth(), getHeight());
	addToDesktop(0);
	toFront(true);
}

void FxSettingsDialog::showTab(int index)
{
	settings_content_.selectTab(index);
}

void FxSettingsDialog::closeButtonPressed()
{
	exitModalState(0);
	removeFromDesktop();
}

void FxSettingsDialog::paint(Graphics& g)
{
	FxWindow::paint(g);

}

void FxSettingsDialog::SettingsButton::paint(Graphics& g)
{
	auto bounds = getLocalBounds().toFloat();
	const bool on = getToggleState();
	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

	g.setColour(on ? FXCOL(HighlightedText) : FXCOL(HintText));
	g.setFont(theme.getNormalFont().withHeight(15.0f));
	g.drawText(TRANS(getName()), bounds.withTrimmedBottom(6.0f), Justification::centred);

	if (on)
	{
		g.setColour(FXCOL(SliderTrack).withAlpha(0.35f));
		g.fillRoundedRectangle(bounds.getX() + 14.0f, bounds.getBottom() - 4.0f, bounds.getWidth() - 28.0f, 4.0f, 2.0f);
		g.setColour(FXCOL(SliderTrack));
		g.fillRoundedRectangle(bounds.getX() + 14.0f, bounds.getBottom() - 3.0f, bounds.getWidth() - 28.0f, 2.0f, 1.0f);
	}
}

bool FxSettingsDialog::keyPressed(const KeyPress& key)
{
	if (key == KeyPress::escapeKey)
	{
		exitModalState(0);
		removeFromDesktop();
		return true;
	}

	return Component::keyPressed(key);
}

FxSettingsDialog::SettingsComponent::SettingsComponent()
{
	audio_button_ = std::make_unique<SettingsButton>("Audio");
	audio_button_->setToggleState(true, NotificationType::dontSendNotification);
	audio_button_->addListener(this);


	general_button_ = std::make_unique<SettingsButton>("General");
	general_button_->setToggleState(false, NotificationType::dontSendNotification);
	general_button_->addListener(this);
	
	help_button_ = std::make_unique<SettingsButton>("Help");
	help_button_->setToggleState(false, NotificationType::dontSendNotification);
	help_button_->addListener(this);    

	addAndMakeVisible(audio_button_.get());
	addAndMakeVisible(general_button_.get());
	addAndMakeVisible(help_button_.get());

	addAndMakeVisible(audio_settings_pane_);
	addChildComponent(general_settings_pane_);
	addChildComponent(help_settings_pane_);

    setSize(WIDTH, HEIGHT);
}

void FxSettingsDialog::SettingsComponent::resized()
{
	audio_button_->setBounds(TAB_X, TAB_Y, TAB_WIDTH, TAB_HEIGHT);
	general_button_->setBounds(audio_button_->getRight() + 6, TAB_Y, TAB_WIDTH, TAB_HEIGHT);
	help_button_->setBounds(general_button_->getRight() + 6, TAB_Y, TAB_WIDTH, TAB_HEIGHT);

	juce::Rectangle<int> pane_rect(0, TAB_BAR_HEIGHT, getWidth(), getHeight() - TAB_BAR_HEIGHT);
	audio_settings_pane_.setBounds(pane_rect);
	general_settings_pane_.setBounds(pane_rect);
	help_settings_pane_.setBounds(pane_rect);
}

void FxSettingsDialog::SettingsComponent::paint(Graphics& g)
{
	g.setColour(Colour(FXCOLOR(Outline)).withAlpha(1.0f));
	g.fillRect(0, TAB_BAR_HEIGHT - 1, getWidth(), 1);
}

void  FxSettingsDialog::SettingsComponent::buttonClicked(Button* button)
{
	if (button == audio_button_.get())
	{
		button->setToggleState(true, NotificationType::dontSendNotification);
		general_button_->setToggleState(false, NotificationType::dontSendNotification);
		help_button_->setToggleState(false, NotificationType::dontSendNotification);

		audio_settings_pane_.setVisible(true);
		general_settings_pane_.setVisible(false);
		help_settings_pane_.setVisible(false);
	}
	else if (button == general_button_.get())
	{
		button->setToggleState(true, NotificationType::dontSendNotification);
		audio_button_->setToggleState(false, NotificationType::dontSendNotification);
		help_button_->setToggleState(false, NotificationType::dontSendNotification);

		general_settings_pane_.setVisible(true);
		audio_settings_pane_.setVisible(false);
		help_settings_pane_.setVisible(false);
	}
	else if (button == help_button_.get())
	{
		button->setToggleState(true, NotificationType::dontSendNotification);
		audio_button_->setToggleState(false, NotificationType::dontSendNotification);
		general_button_->setToggleState(false, NotificationType::dontSendNotification);

		help_settings_pane_.setVisible(true);
		audio_settings_pane_.setVisible(false);
		general_settings_pane_.setVisible(false);
	}
}

FxSettingsDialog::SettingsPane::SettingsPane(String name)
{
	name_ = name;
	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

    title_.setFont(theme.getTitleFont());
	title_.setText(TRANS(name_), NotificationType::dontSendNotification);	
	title_.setColour(Label::ColourIds::textColourId, getLookAndFeel().findColour(TextButton::textColourOnId));
	title_.setJustificationType(Justification::centredLeft);
	// The tab already names the panel.
}

void FxSettingsDialog::SettingsPane::paint(Graphics&)
{
    auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

    title_.setFont(theme.getTitleFont());
    title_.setText(TRANS(name_), NotificationType::dontSendNotification);    
}

FxSettingsDialog::AudioSettingsPane::AudioSettingsPane() :
	SettingsPane("Audio"),
	prioritize_new_output_toggle_(TRANS("Prioritize new output devices")),
	reset_presets_button_(TRANS("Reset presets to factory defaults"))
{
	setFocusContainer(true);

	output_preference_title_.setColour(Label::ColourIds::textColourId, getLookAndFeel().findColour(TextButton::textColourOnId));
	output_preference_title_.setJustificationType(Justification::centredLeft);

	output_preference_.setMouseCursor(MouseCursor::PointingHandCursor);
	output_preference_.setWantsKeyboardFocus(true);
	output_preference_.setEnabled(true);

	prioritize_new_output_toggle_.setMouseCursor(MouseCursor::PointingHandCursor);
	prioritize_new_output_toggle_.setColour(ToggleButton::ColourIds::tickColourId, getLookAndFeel().findColour(TextButton::textColourOnId));
	prioritize_new_output_toggle_.setColour(ToggleButton::ColourIds::textColourId, getLookAndFeel().findColour(TextButton::textColourOnId));
	prioritize_new_output_toggle_.setWantsKeyboardFocus(true);

	max_boost_title_.setColour(Label::ColourIds::textColourId, getLookAndFeel().findColour(TextButton::textColourOnId));
	max_boost_title_.setJustificationType(Justification::centredLeft);

	static const int boost_options[5] = { 6, 10, 15, 20, 30 };
	const int current_boost = FxController::getInstance().getMaxBoostDb();
	int selected = 0;
	for (int i = 1; i < 5; i++)
		if (std::abs(boost_options[i] - current_boost) < std::abs(boost_options[selected] - current_boost))
			selected = i;
	for (int i = 0; i < 5; i++)
	{
		auto& b = max_boost_buttons_[i];
		b.setButtonText("+" + String(boost_options[i]) + " dB");
		b.setClickingTogglesState(true);
		b.setRadioGroupId(7001);
		b.setMouseCursor(MouseCursor::PointingHandCursor);
		b.setColour(TextButton::buttonColourId, FXCOL(WidgetBackground));
		b.setColour(TextButton::buttonOnColourId, FXCOL(SliderTrack));
		b.setColour(TextButton::textColourOffId, FXCOL(DefaultText));
		b.setColour(TextButton::textColourOnId, FXCOL(WindowBackground));
		b.setToggleState(i == selected, NotificationType::dontSendNotification);
		b.onClick = [i]() { FxController::getInstance().setMaxBoostDb(boost_options[i]); };
		addAndMakeVisible(b);
	}

	reset_presets_button_.setSize(RESET_PRESETS_BUTTON_WIDTH, BUTTON_HEIGHT);
	reset_presets_button_.setMouseCursor(MouseCursor::PointingHandCursor);

	prioritize_new_output_toggle_.setToggleState(FxController::getInstance().isNewOutputPrioritized(), NotificationType::dontSendNotification);
	prioritize_new_output_toggle_.onClick = [this]() { FxController::getInstance().setNewOutputPrioritized(prioritize_new_output_toggle_.getToggleState()); };

	auto preset_modified = false;
	auto preset_count = FxModel::getModel().getPresetCount();
	for (auto i = 0; i < preset_count; i++)
	{
		if (FxModel::getModel().isPresetModified(i))
		{
			preset_modified = true;
			break;
		}
	}
	reset_presets_button_.setEnabled(FxModel::getModel().getUserPresetCount() > 0 || preset_modified);

	reset_presets_button_.onClick = [this]() {
		auto& controller = FxController::getInstance();

		controller.resetPresets();
		reset_presets_button_.setEnabled(false);
		};

	setText();

	addAndMakeVisible(&output_preference_title_);
	addAndMakeVisible(&output_preference_);
	addAndMakeVisible(&prioritize_new_output_toggle_);
	addAndMakeVisible(&max_boost_title_);
	addAndMakeVisible(&reset_presets_button_);
}

FxSettingsDialog::AudioSettingsPane::~AudioSettingsPane()
{
}

void FxSettingsDialog::AudioSettingsPane::resized()
{
	auto bounds = getLocalBounds().withLeft(X_MARGIN).withTop(Y_MARGIN).withHeight(TITLE_HEIGHT);
	title_.setBounds(bounds);

	output_preference_title_.setBounds(X_MARGIN, ENDPOINT_Y, LABEL_WIDTH, LABEL_HEIGHT);
	int y = output_preference_title_.getBottom() + 10;
	auto width = getWidth() - ((X_MARGIN + 5) * 2);
	output_preference_.setBounds(X_MARGIN, y, width, OUTPUT_PREFERENCE_HEIGHT);

    y = output_preference_.getBottom() + 10;
    prioritize_new_output_toggle_.setBounds(X_MARGIN, y, width, TOGGLE_BUTTON_HEIGHT);

	auto group_x = output_preference_title_.getX() - GROUP_MARGIN;
	auto group_y = output_preference_title_.getY() - GROUP_MARGIN;
	auto group_width = output_preference_.getRight() - group_x + GROUP_MARGIN;
	auto group_height = prioritize_new_output_toggle_.getBottom() - group_y + GROUP_MARGIN;
	output_preference_bounds_ = juce::Rectangle<float>(group_x, group_y, group_width, group_height);

	y = prioritize_new_output_toggle_.getBottom() + 30;
	max_boost_title_.setBounds(X_MARGIN, y, width, LABEL_HEIGHT + 6);
	{
		const int gap = 6;
		const int bw = (width - gap * 4) / 5;
		for (int i = 0; i < 5; i++)
			max_boost_buttons_[i].setBounds(X_MARGIN + i * (bw + gap), max_boost_title_.getBottom() + 6, bw, 30);
	}
	y = max_boost_buttons_[0].getBottom() + 22;
	resizeResetButton(X_MARGIN, y);
}

void FxSettingsDialog::AudioSettingsPane::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	setText();

	SettingsPane::paint(g);
}

void FxSettingsDialog::AudioSettingsPane::setText()
{
	auto& theme = dynamic_cast<FxTheme&>(LookAndFeel::getDefaultLookAndFeel());

	output_preference_title_.setFont(theme.getNormalFont());
	output_preference_title_.setText(TRANS("Output Device Preference"), NotificationType::dontSendNotification);

	prioritize_new_output_toggle_.setButtonText(TRANS("Prioritize new output devices"));
	max_boost_title_.setFont(theme.getNormalFont());
	max_boost_title_.setText(TRANS("Power limit (how far the gain slider can go)"), NotificationType::dontSendNotification);

	reset_presets_button_.setButtonText(TRANS("Reset presets to factory defaults"));
	resizeResetButton(reset_presets_button_.getX(), reset_presets_button_.getY());
}

void FxSettingsDialog::AudioSettingsPane::resizeResetButton(int x, int y)
{
	String buttonText = reset_presets_button_.getButtonText();

	int index = 0;
	int lineCount = 1;
	do {
		index = buttonText.indexOfChar(index, L'\n');
		if (index >= 0)
		{
			index++;
			lineCount++;
		}
		else
		{
			break;
		}
	} while (lineCount <= 3); // Resize the button height for upto 3 lines of text

	int buttonWidth = min(reset_presets_button_.getBestWidthForHeight(BUTTON_HEIGHT * lineCount), MAX_BUTTON_WIDTH);
	if (buttonWidth < RESET_PRESETS_BUTTON_WIDTH)
	{
		buttonWidth = RESET_PRESETS_BUTTON_WIDTH;
	}

	reset_presets_button_.setBounds(x, y, buttonWidth, BUTTON_HEIGHT * lineCount);
}

void FxSettingsDialog::AudioSettingsPane::visibilityChanged()
{
	if (isVisible())
	{
		output_preference_.update();
    }
}

void FxSettingsDialog::AudioSettingsPane::mouseEnter(const MouseEvent& mouse_event)
{
	Component::mouseEnter(mouse_event);
}

void FxSettingsDialog::AudioSettingsPane::mouseExit(const MouseEvent& mouse_event)
{
	Component::mouseEnter(mouse_event);
}

namespace
{
	// Familia de sistema con los glifos de cada idioma (la tipografía de la app solo cubre alfabetos latinos y cirílico).
	Font nativeFont(const String& code, float height)
	{
		auto family = FxTheme::fontFamilyForLanguage(code);
		if (family.isEmpty())
			return dynamic_cast<FxTheme&>(LookAndFeel::getDefaultLookAndFeel()).getNormalFont().withHeight(height);
		return Font(family, height, Font::plain);
	}

	// Fila del menú de idiomas: el nombre se escribe con una tipografía que sí tiene sus letras.
	class FxLanguageItem : public PopupMenu::CustomComponent
	{
	public:
		FxLanguageItem(const String& code, const String& name, bool current) : PopupMenu::CustomComponent(true), code_(code), name_(name), current_(current) {}

		void getIdealSize(int& w, int& h) override { w = 196; h = 34; }

		void paint(Graphics& g) override
		{
			auto r = getLocalBounds().toFloat().reduced(4.0f, 2.0f);
			if (isItemHighlighted())
			{
				g.setColour(FXCOL(SliderTrack).withAlpha(0.22f));
				g.fillRoundedRectangle(r, 8.0f);
			}
			g.setColour(current_ ? FXCOL(SliderTrack).brighter(0.35f) : FXCOL(HighlightedText));
			g.setFont(nativeFont(code_, 15.0f));
			g.drawText(name_, r.withTrimmedLeft(12.0f).withTrimmedRight(26.0f), Justification::centredLeft, true);
			if (current_)
			{
				Path tick; const float x = r.getRight() - 18.0f, y = r.getCentreY();
				tick.startNewSubPath(x - 4.5f, y); tick.lineTo(x - 1.0f, y + 3.5f); tick.lineTo(x + 5.0f, y - 4.0f);
				g.setColour(FXCOL(SliderTrack).brighter(0.35f));
				g.strokePath(tick, PathStrokeType(1.8f, PathStrokeType::curved, PathStrokeType::rounded));
			}
		}

	private:
		String code_, name_;
		bool current_;
	};

	String hotkeyText(int mod, int vk)
	{
		if (vk == 0) return String();
		String s;
		if (mod & MOD_CONTROL) s << "Ctrl + ";
		if (mod & MOD_ALT) s << "Alt + ";
		if (mod & MOD_SHIFT) s << "Shift + ";
		if (vk >= VK_F1 && vk <= VK_F12) s << "F" << String(vk - VK_F1 + 1);
		else if (vk == VK_LEFT) s << String::charToString(0x2190);
		else if (vk == VK_UP) s << String::charToString(0x2191);
		else if (vk == VK_RIGHT) s << String::charToString(0x2192);
		else if (vk == VK_DOWN) s << String::charToString(0x2193);
		else s << String::charToString((juce_wchar)vk);
		return s;
	}

	const char* const LANGUAGE_CODES[] = { "en", "es", "pt", "pt-br", "fr", "de", "it", "nl", "sv", "no", "fi", "pl", "cs", "hr", "sl", "ro", "hu", "tr", "ru", "ua", "ba", "id", "vi", "th", "ja", "ko", "zh-CN", "zh-TW", "ar", "fa" };
}

FxSettingsDialog::GeneralSettingsPane::GeneralSettingsPane() :
	SettingsPane("General Preferences"),
	language_row_(FxSettingRow::Kind::Value, "Interface language"),
	help_tips_row_(FxSettingRow::Kind::Switch, "Hide help tips"),
	notifications_row_(FxSettingRow::Kind::Switch, "Hide notifications"),
	startup_row_(FxSettingRow::Kind::Switch, "Launch on system startup"),
	hotkeys_row_(FxSettingRow::Kind::Switch, "Enable shortcuts", "Work even when Nubetalo Sound is in the background"),
	restore_row_(FxSettingRow::Kind::Link, "Restore default shortcuts")
{
	auto& controller = FxController::getInstance();

	shortcut_cmds_ = { FxController::HK_CMD_ON_OFF, FxController::HK_CMD_OPEN_CLOSE, FxController::HK_CMD_NEXT_PRESET, FxController::HK_CMD_PREVIOUS_PRESET, FxController::HK_CMD_NEXT_OUTPUT };
	const StringArray names = { "Turn Nubetalo Sound On/Off", "Open/Close Nubetalo Sound", "Use Next Preset", "Use Previous Preset", "Change Playback Device" };

	for (int i = 0; i < shortcut_cmds_.size(); i++)
	{
		auto* row = new FxSettingRow(FxSettingRow::Kind::Key, names[i]);
		row->onKeyPress = [this, i](const KeyPress& key) { return setShortcut(i, key); };
		shortcut_rows_.add(row);
		addAndMakeVisible(row);
	}

	language_row_.onClick = [this]() { showLanguageMenu(); };

	help_tips_row_.setOn(controller.isHelpTooltipsHidden());
	help_tips_row_.onClick = [this]() { FxController::getInstance().setHelpTooltipsHidden(help_tips_row_.isOn()); };

	notifications_row_.setOn(controller.isNotificationsHidden());
	notifications_row_.onClick = [this]() { FxController::getInstance().setNotificationsHidden(notifications_row_.isOn()); };

	startup_row_.setOn(controller.isLaunchOnStartup());
	startup_row_.onClick = [this]() { FxController::getInstance().setLaunchOnStartup(startup_row_.isOn()); };

	const bool can_hotkeys = SysInfo::canSupportHotkeys();
	hotkeys_row_.setOn(can_hotkeys && FxModel::getModel().getHotkeySupport());
	hotkeys_row_.setEnabled(can_hotkeys);
	hotkeys_row_.onClick = [this]() {
		FxController::getInstance().enableHotkeys(hotkeys_row_.isOn());
		refreshShortcuts();
	};

	restore_row_.onClick = [this]() {
		FxController::getInstance().resetHotkeys();
		refreshShortcuts();
	};

	for (auto* c : { &language_row_, &help_tips_row_, &notifications_row_, &startup_row_, &hotkeys_row_, &restore_row_ })
		addAndMakeVisible(c);

	refreshShortcuts();
	refreshTexts();
}

void FxSettingsDialog::GeneralSettingsPane::refreshTexts()
{
	auto& controller = FxController::getInstance();
	const auto code = controller.getLanguage();
	language_row_.setValue(controller.getLanguageName(code));
	language_row_.setValueFont(nativeFont(code, 14.0f));
	repaint();
}

void FxSettingsDialog::GeneralSettingsPane::refreshShortcuts()
{
	auto& controller = FxController::getInstance();
	const bool enabled = FxModel::getModel().getHotkeySupport();
	for (int i = 0; i < shortcut_rows_.size(); i++)
	{
		int mod = 0, vk = 0;
		controller.getHotkey(shortcut_cmds_[i], mod, vk);
		shortcut_rows_[i]->setValue(hotkeyText(mod, vk));
		shortcut_rows_[i]->setEnabled(enabled);
	}
	restore_row_.setEnabled(enabled);
	repaint();
}

// Acepta Ctrl + Alt y/o Shift con una letra, un número, F1-F12 o una flecha. Supr/Retroceso borran el atajo.
bool FxSettingsDialog::GeneralSettingsPane::setShortcut(int index, const KeyPress& key)
{
	auto& controller = FxController::getInstance();
	const auto cmd = shortcut_cmds_[index];

	if (key == KeyPress::deleteKey || key == KeyPress::backspaceKey)
	{
		controller.setHotkey(cmd, 0, 0);
		refreshShortcuts();
		return true;
	}

	const auto m = key.getModifiers();
	int mod = 0;
	if (m.isCtrlDown()) mod |= MOD_CONTROL;
	if (m.isAltDown()) mod |= MOD_ALT;
	if (m.isShiftDown()) mod |= MOD_SHIFT;
	if ((mod & MOD_CONTROL) == 0 || (mod & (MOD_ALT | MOD_SHIFT)) == 0)
		return false; // hace falta Ctrl + Alt o Shift: así no se pisan las teclas de otras aplicaciones

	int vk = 0;
	const int code = key.getKeyCode();
	if ((code >= '0' && code <= '9') || (code >= 'A' && code <= 'Z')) vk = code;
	else if (code >= KeyPress::F1Key && code <= KeyPress::F12Key) vk = VK_F1 + (code - KeyPress::F1Key);
	else if (code == KeyPress::leftKey) vk = VK_LEFT;
	else if (code == KeyPress::rightKey) vk = VK_RIGHT;
	else if (code == KeyPress::upKey) vk = VK_UP;
	else if (code == KeyPress::downKey) vk = VK_DOWN;
	if (vk == 0)
		return false;

	if (!controller.setHotkey(cmd, mod, vk))
	{
		AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, TRANS("Shortcut not available"),
			TRANS("That combination is already used by another shortcut or by another program. Try a different one."));
	}
	refreshShortcuts();
	return true;
}

void FxSettingsDialog::GeneralSettingsPane::showLanguageMenu()
{
	auto& controller = FxController::getInstance();
	const auto current = controller.getLanguage();

	PopupMenu menu;
	int id = 1;
	for (auto* code : LANGUAGE_CODES)
	{
		PopupMenu::Item item;
		item.itemID = id++;
		item.customComponent = new FxLanguageItem(code, controller.getLanguageName(code), current.startsWithIgnoreCase(code) && (String(code) != "pt" || !current.startsWithIgnoreCase("pt-br")));
		item.action = [this, code = String(code)]() {
			FxController::getInstance().setLanguage(code);
			refreshTexts();
			if (auto* top = getTopLevelComponent()) top->repaint();
		};
		menu.addItem(item);
	}

	menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&language_row_).withMinimumWidth(196).withMaximumNumColumns(3).withStandardItemHeight(34));
}

void FxSettingsDialog::GeneralSettingsPane::resized()
{
	const int margin = 24, gap = 24;
	const int col_w = (getWidth() - margin * 2 - gap) / 2;
	col_x_[0] = margin;
	col_x_[1] = margin + col_w + gap;

	// columna izquierda: idioma y preferencias
	int y = 12;
	section_y_[0] = y; y += SECTION_H;
	language_row_.setBounds(col_x_[0], y, col_w, ROW_H); y += ROW_H + 18;
	section_y_[1] = y; y += SECTION_H;
	for (auto* r : { &help_tips_row_, &notifications_row_, &startup_row_ })
	{
		r->setBounds(col_x_[0], y, col_w, ROW_H);
		y += ROW_H + GAP;
	}

	// columna derecha: atajos
	y = 12;
	section_y_[2] = y; y += SECTION_H;
	hotkeys_row_.setBounds(col_x_[1], y, col_w, 56); y += 56 + GAP;
	for (auto* r : shortcut_rows_)
	{
		r->setBounds(col_x_[1], y, col_w, ROW_H);
		y += ROW_H + GAP;
	}
	restore_row_.setBounds(col_x_[1], y + 4, col_w, ROW_H);
}

void FxSettingsDialog::GeneralSettingsPane::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
	g.setFont(theme.getNormalFont().withHeight(13.0f));
	g.setColour(FXCOL(HintText));
	g.drawText(TRANS("Language"), col_x_[0] + 2, section_y_[0], 300, SECTION_H, Justification::centredLeft);
	g.drawText(TRANS("Preferences"), col_x_[0] + 2, section_y_[1], 300, SECTION_H, Justification::centredLeft);
	g.drawText(TRANS("Keyboard shortcuts"), col_x_[1] + 2, section_y_[2], 300, SECTION_H, Justification::centredLeft);
}

//==============================================================================
FxSettingsDialog::HelpSettingsPane::HelpSettingsPane() :
	SettingsPane("Help"),
	mail_row_(FxSettingRow::Kind::Link, "Write to support", FxSupport::email()),
	bug_row_(FxSettingRow::Kind::Link, "Report a problem", "Opens an email with your version and system"),
	copy_row_(FxSettingRow::Kind::Link, "Copy system information", "To paste it in an email or a report"),
	donate_row_(FxSettingRow::Kind::Link, "Donate", "PayPal"),
	license_row_(FxSettingRow::Kind::Link, "License", "GNU Affero General Public License v3"),
	notices_row_(FxSettingRow::Kind::Link, "Third-party notices", "FxSound, JUCE and IBM Plex Sans")
{
	mail_row_.onClick = []() { FxSupport::openMail("Ayuda"); };
	bug_row_.onClick = []() { FxSupport::openMail("Problema", true); };
	copy_row_.onClick = [this]() {
		SystemClipboard::copyTextToClipboard(FxSupport::systemInfo());
		copy_row_.setSubtitle("Copied");
		Timer::callAfterDelay(1800, [safe = Component::SafePointer<FxSettingRow>(&copy_row_)]() { if (safe != nullptr) safe->setSubtitle("To paste it in an email or a report"); });
	};
	donate_row_.onClick = []() { FxDonate::open(); };

	auto openDoc = [](const String& name) {
		auto f = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getChildFile(name);
		if (f.existsAsFile()) f.startAsProcess();
		else AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon, name, TRANS("This file is installed with Nubetalo Sound, in its program folder."));
	};
	license_row_.onClick = [openDoc]() { openDoc("LICENSE.txt"); };
	notices_row_.onClick = [openDoc]() { openDoc("NOTICE.txt"); };

	for (auto* c : { &mail_row_, &bug_row_, &copy_row_, &donate_row_, &license_row_, &notices_row_ })
		addAndMakeVisible(c);
}

void FxSettingsDialog::HelpSettingsPane::resized()
{
	const int margin = 24, gap = 24;
	const int col_w = (getWidth() - margin * 2 - gap) / 2;
	col_x_[0] = margin;
	col_x_[1] = margin + col_w + gap;

	int y = 12 + HEADER_H + 14;
	section_y_ = y; y += SECTION_H;
	int y0 = y;
	for (auto* r : { &mail_row_, &bug_row_, &copy_row_ })
	{
		r->setBounds(col_x_[0], y, col_w, ROW_H + 8);
		y += ROW_H + 8 + GAP;
	}
	y = y0;
	for (auto* r : { &donate_row_, &license_row_, &notices_row_ })
	{
		r->setBounds(col_x_[1], y, col_w, ROW_H + 8);
		y += ROW_H + 8 + GAP;
	}
}

void FxSettingsDialog::HelpSettingsPane::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

	// tarjeta de versión
	const auto card = juce::Rectangle<float>((float)col_x_[0], 12.0f, (float)(getWidth() - col_x_[0] * 2), (float)HEADER_H);
	g.setColour(FXCOL(ControlBackground));
	g.fillRoundedRectangle(card, 12.0f);
	g.setColour(FXCOL(Outline));
	g.drawRoundedRectangle(card.reduced(0.5f), 12.0f, 1.0f);

	if (auto logo = Drawable::createFromImageData(FXIMAGE(IconLogo), FXIMAGESIZE(IconLogo)))
		logo->drawWithin(g, juce::Rectangle<float>(card.getX() + 22.0f, card.getCentreY() - 25.0f, 50.0f, 50.0f), RectanglePlacement::centred, 1.0f);

	g.setColour(FXCOL(HighlightedText));
	g.setFont(theme.getNormalFont().withHeight(20.0f));
	g.drawText("Nubetalo Sound", (int)card.getX() + 90, (int)card.getY() + 18, 400, 28, Justification::centredLeft);
	g.setColour(FXCOL(HintText));
	g.setFont(theme.getNormalFont().withHeight(13.5f));
	g.drawText(TRANS("Version") + " " + FxSupport::versionText(), (int)card.getX() + 90, (int)card.getY() + 48, 400, 20, Justification::centredLeft);
	g.drawText(TRANS("Open-source audio enhancer for Windows"), (int)card.getX() + 90, (int)card.getY() + 67, 500, 20, Justification::centredLeft);

	g.setFont(theme.getNormalFont().withHeight(13.0f));
	g.drawText(TRANS("Support"), col_x_[0] + 2, section_y_, 300, SECTION_H, Justification::centredLeft);
	g.drawText(TRANS("Project"), col_x_[1] + 2, section_y_, 300, SECTION_H, Justification::centredLeft);
}
