"""Reemplaza las pantallas General y Soporte de Ajustes por el diseño nuevo."""
G = 'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/Source/GUI/'

def load(p):
    b = open(G + p, 'rb').read()
    return b.decode('utf-8-sig').replace('\r\n', '\n'), b.startswith(b'\xef\xbb\xbf')

def save(p, t, bom):
    open(G + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.replace('\n', '\r\n').encode('utf-8'))

# ---------------- cabecera ----------------
h, bom = load('FxSettingsDialog.h')
i = h.index('\tclass GeneralSettingsPane : public SettingsPane')
j = h.index('\tclass SettingsComponent : public Component')
new_h = '''	class GeneralSettingsPane : public SettingsPane
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
		int section_y_ = 0;
		int col_x_[2] = { 0, 0 };
	};

'''
h = h[:i] + new_h + h[j:]
h = h.replace('static constexpr int WIDTH = 600;', 'static constexpr int WIDTH = 720;')
if 'FxSettingRow.h' not in h:
    h = h.replace('#include "FxOutputPreference.h"', '#include "FxOutputPreference.h"\n#include "FxSettingRow.h"')
assert 'FxSettingRow.h' in h
save('FxSettingsDialog.h', h, bom)

# ---------------- implementación ----------------
c, bom = load('FxSettingsDialog.cpp')
i = c.index('FxSettingsDialog::GeneralSettingsPane::GeneralSettingsPane()')
new_c = r'''namespace
{
	// Familia de sistema con los glifos de cada idioma (la tipografía de la app solo cubre alfabetos latinos y cirílico).
	Font nativeFont(const String& code, float height)
	{
		auto family = FxTheme::fontFamilyForLanguage(code);
		return family.isEmpty() ? FxTheme::getDefaultUiFont(height) : Font(family, height, Font::plain);
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
	language_row_(FxSettingRow::Kind::Value, "Language"),
	help_tips_row_(FxSettingRow::Kind::Switch, "Hide help tips for audio controls"),
	notifications_row_(FxSettingRow::Kind::Switch, "Hide notifications"),
	startup_row_(FxSettingRow::Kind::Switch, "Launch on system startup"),
	hotkeys_row_(FxSettingRow::Kind::Switch, "Keyboard shortcuts", "Work even when Nubetalo Sound is in the background"),
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
	donate_row_(FxSettingRow::Kind::Link, "Donate", "paypal.me/Nubetalo"),
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
'''
c = c[:i] + new_c
for inc in ('#include "FxSupport.h"', '#include "FxDonate.h"'):
    if inc not in c:
        c = c.replace('#include "FxSettingsDialog.h"', '#include "FxSettingsDialog.h"\n' + inc, 1)
save('FxSettingsDialog.cpp', c, bom)
print('ok')
