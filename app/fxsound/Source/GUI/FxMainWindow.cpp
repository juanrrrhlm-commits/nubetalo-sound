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
#include "FxMainWindow.h"
#include "FxController.h"
#include "FxSettingsDialog.h"
#include "FxPresetExportDialog.h"
#include "FxPresetImportDialog.h"
#include "FxPresetNameEditor.h"
#include "FxDonate.h"

class FxPresetMenuItem : public PopupMenu::CustomComponent, private TextEditor::Listener
{
public:
	enum class Status { Empty = 0, Valid, Invalid };
	enum class Action { Save = 1, Rename };

	FxPresetMenuItem(Action action) : PopupMenu::CustomComponent(false)
	{
		auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

		preset_status_ = Status::Empty;

		hint_text_.setName(L"hintLabel");
		hint_text_.setFont(theme.getNormalFont());
		hint_text_.setColour(Label::ColourIds::textColourId, Colour(FXCOLOR(HintText)).withAlpha(1.0f));
		hint_text_.setJustificationType(Justification::centredLeft);
		addAndMakeVisible(hint_text_);

		if (action == Action::Save)
		{
			hint_text_.setText(TRANS("Enter your preset name"), NotificationType::dontSendNotification);
			preset_editor_.setDescription(TRANS("Enter your preset name"));
		}
		else
		{
			hint_text_.setText(TRANS("Enter new preset name"), NotificationType::dontSendNotification);
			preset_editor_.setDescription(TRANS("Enter new preset name"));
		}
		preset_editor_.setName(L"presetName");
		preset_editor_.setFont(theme.getNormalFont());
		preset_editor_.setColour(TextEditor::ColourIds::backgroundColourId, Colour(FXCOLOR(DefaultFill)).withAlpha(0.0f));
		preset_editor_.setInputRestrictions(64);
		preset_editor_.setInputFilter(&preset_name_input_filter_, false);
		preset_editor_.addListener(this);
		addAndMakeVisible(preset_editor_);

		preset_editor_.onEscapeKey = [this]() {
			preset_status_ = Status::Empty;
			triggerMenuItem();
		};

		preset_editor_.onReturnKey = [this, action]() {
			if (preset_status_ == Status::Valid)
			{
				if (action == Action::Save)
				{
					FxController::getInstance().savePreset(preset_name_);
					triggerMenuItem();
				}
				else
				{
					FxController::getInstance().renamePreset(preset_name_);
					triggerMenuItem();
				}
			}
		};
	}
	~FxPresetMenuItem() = default;

	Status getStatus() { return preset_status_; }
	String getPresetName() { return preset_name_; }

	void getIdealSize(int& ideal_width, int& ideal_height)
	{
		ideal_width = WIDTH;
		ideal_height = HEIGHT;
	}

private:
	static constexpr int WIDTH = 200;
	static constexpr int HEIGHT = 30;

	void resized() override
	{
		auto bounds = getLocalBounds().reduced(2, 2);

		hint_text_.setBounds(bounds);
		preset_editor_.setBounds(bounds);
	}

	void paint(Graphics& g) override
	{
		auto bounds = getLocalBounds();
		Colour outline_colour;

		switch (preset_status_)
		{
		case Status::Valid:
			outline_colour = Colour(FXCOLOR(ValidTextBorder)).withAlpha(1.0f);
			break;

		case Status::Empty:
		case Status::Invalid:
			outline_colour = Colour(FXCOLOR(InvalidTextBorder)).withAlpha(1.0f);
		}

		g.setColour(findColour(TextEditor::backgroundColourId));
		g.fillRect(bounds.toFloat());

		g.setColour(outline_colour);
		g.drawRect(bounds.toFloat().reduced(0.5f, 0.5f), 2.0f);

		preset_editor_.grabKeyboardFocus();
	}

	void textEditorTextChanged(TextEditor& textEditor) override
	{
		auto prevStatus = preset_status_;

		preset_name_ = textEditor.getText();
		if (preset_name_.isEmpty())
		{
			preset_status_ = Status::Empty;
			hint_text_.setAlpha(1.0);
		}
		else
		{
			hint_text_.setAlpha(0.0);

			preset_status_ = Status::Valid;
			auto& model = FxModel::getModel();
			for (auto i = 0; i < model.getPresetCount(); i++)
			{
				if (model.getPreset(i).name.equalsIgnoreCase(preset_name_))
				{
					preset_status_ = Status::Invalid;
					break;
				}
			}
		}

		if (prevStatus != preset_status_)
		{
			sendLookAndFeelChange();
		}
	}

	void lookAndFeelChanged() override
	{
		repaint();
	}

	TextEditor preset_editor_;
	PresetNameInputFilter preset_name_input_filter_;
	Label hint_text_;
	Status preset_status_;
	String preset_name_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxPresetMenuItem)
};

// Modal dialog to name a new preset or rename the current one.
class FxPresetNameDialog : public FxWindow
{
public:
	enum class Mode { Save, Rename };

	FxPresetNameDialog(Mode mode)
		: FxWindow(mode == Mode::Save ? "Save New Preset" : "Rename Preset"),
		  content_(mode, [this](bool ok) { accepted_ = ok; exitModalState(ok ? 1 : 0); removeFromDesktop(); })
	{
		setContent(&content_);
		centreWithSize(getWidth(), getHeight());
		addToDesktop(0);
		toFront(true);
	}

	String getName() const { return content_.getName(); }
	void closeButtonPressed() override { exitModalState(0); removeFromDesktop(); }

	bool keyPressed(const KeyPress& key) override
	{
		if (key == KeyPress::escapeKey) { closeButtonPressed(); return true; }
		return FxWindow::keyPressed(key);
	}

private:
	class Content : public Component, private TextEditor::Listener
	{
	public:
		Content(Mode mode, std::function<void(bool)> done) : mode_(mode), done_(done)
		{
			auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

			heading_.setText(TRANS(mode == Mode::Save ? "Name your new preset" : "Give this preset a new name"), NotificationType::dontSendNotification);
			heading_.setFont(theme.getTitleFont().withHeight(20.0f));
			heading_.setColour(Label::textColourId, FXCOL(HighlightedText));
			addAndMakeVisible(heading_);

			editor_.setFont(theme.getNormalFont().withHeight(18.0f));
			editor_.setColour(TextEditor::backgroundColourId, Colour(0x00000000));
			editor_.setColour(TextEditor::outlineColourId, Colour(0x00000000));
			editor_.setColour(TextEditor::focusedOutlineColourId, Colour(0x00000000));
			editor_.setColour(TextEditor::textColourId, FXCOL(HighlightedText));
			editor_.setColour(CaretComponent::caretColourId, FXCOL(SliderTrack));
			editor_.setIndents(0, 0);
			editor_.setInputRestrictions(64);
			editor_.setInputFilter(&filter_, false);
			editor_.setTextToShowWhenEmpty(TRANS("e.g. My favourite sound"), FXCOL(HintText));
			editor_.addListener(this);
			editor_.onReturnKey = [this]() { accept(); };
			editor_.onEscapeKey = [this]() { done_(false); };
			if (mode == Mode::Rename)
			{
				auto& model = FxModel::getModel();
				editor_.setText(model.getPreset(model.getSelectedPreset()).name, false);
			}
			addAndMakeVisible(editor_);

			status_.setFont(theme.getNormalFont().withHeight(13.0f));
			addAndMakeVisible(status_);

			cancel_.setButtonText(TRANS("Cancel"));
			cancel_.setColour(TextButton::buttonColourId, FXCOL(WidgetBackground));
			cancel_.setColour(TextButton::textColourOffId, FXCOL(DefaultText));
			cancel_.setMouseCursor(MouseCursor::PointingHandCursor);
			cancel_.onClick = [this]() { done_(false); };
			addAndMakeVisible(cancel_);

			ok_.setButtonText(TRANS(mode == Mode::Save ? "Save" : "Rename"));
			ok_.setColour(TextButton::buttonColourId, FXCOL(SliderTrack));
			ok_.setColour(TextButton::textColourOffId, FXCOL(HighlightedText));
			ok_.setMouseCursor(MouseCursor::PointingHandCursor);
			ok_.onClick = [this]() { accept(); };
			addAndMakeVisible(ok_);

			refreshStatus();
			setSize(W, H);
		}

		String getName() const { return editor_.getText().trim(); }

		void parentHierarchyChanged() override { if (isShowing()) editor_.grabKeyboardFocus(); }

		void resized() override
		{
			heading_.setBounds(PAD, 22, W - PAD * 2, 30);
			editor_.setBounds(PAD + 16, 214, W - PAD * 2 - 32, 46);
			status_.setBounds(PAD + 4, 268, W - PAD * 2, 18);
			ok_.setBounds(W - PAD - 118, H - 62, 118, 40);
			cancel_.setBounds(ok_.getX() - 10 - 110, H - 62, 110, 40);
		}

		void paint(Graphics& g) override
		{
			const auto preview = juce::Rectangle<float>((float)PAD, 62.0f, (float)(W - PAD * 2), 96.0f);
			g.setColour(FXCOL(ControlBackground).brighter(0.02f));
			g.fillRoundedRectangle(preview, 14.0f);

			auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
			g.setFont(theme.getNormalFont().withHeight(12.0f));
			g.setColour(FXCOL(HintText));
			g.drawText(TRANS("Current settings"), preview.reduced(16.0f, 8.0f).removeFromTop(16.0f), Justification::centredLeft);

			// mini equalizer curve of what is about to be saved
			auto& controller = FxController::getInstance();
			const int bands = controller.getNumEqBands();
			if (bands > 1)
			{
				auto area = preview.reduced(20.0f, 0.0f).withTrimmedTop(28.0f).withTrimmedBottom(12.0f);
				Path line;
				for (int i = 0; i < bands; i++)
				{
					const float v = jlimit(-6.0f, 6.0f, controller.getEqBandBoostCut(i)) / 6.0f;
					const float x = area.getX() + area.getWidth() * i / (float)(bands - 1);
					const float y = area.getCentreY() - v * area.getHeight() * 0.5f;
					if (i == 0) line.startNewSubPath(x, y);
					else
					{
						const float px = area.getX() + area.getWidth() * (i - 1) / (float)(bands - 1);
						const float py = line.getCurrentPosition().y;
						line.cubicTo(px + (x - px) * 0.5f, py, px + (x - px) * 0.5f, y, x, y);
					}
				}
				Path fill(line);
				fill.lineTo(area.getRight(), area.getBottom());
				fill.lineTo(area.getX(), area.getBottom());
				fill.closeSubPath();
				g.setGradientFill(ColourGradient(FXCOL(SliderTrack).withAlpha(0.28f), 0.0f, area.getY(), FXCOL(SliderTrack).withAlpha(0.0f), 0.0f, area.getBottom(), false));
				g.fillPath(fill);
				g.setColour(FXCOL(Outline));
				g.drawLine(area.getX(), area.getCentreY(), area.getRight(), area.getCentreY(), 1.0f);
				g.setColour(FXCOL(SliderTrack).withAlpha(0.25f));
				g.strokePath(line, PathStrokeType(5.0f, PathStrokeType::curved, PathStrokeType::rounded));
				g.setColour(FXCOL(SliderTrack));
				g.strokePath(line, PathStrokeType(1.8f, PathStrokeType::curved, PathStrokeType::rounded));
			}

			// effect levels, one line
			StringArray names = { TRANS("Clarity"), TRANS("Ambience"), TRANS("Surround Sound"), TRANS("Dynamic Boost"), TRANS("Bass Boost") };
			String text;
			for (int i = 0; i < 5; i++)
			{
				if (i > 0) text += "   ";
				text += names[i] + " " + String(roundToInt(controller.getEffectValue(static_cast<FxEffects::EffectType>(i)) * 10.0f));
			}
			g.setFont(theme.getNormalFont().withHeight(13.0f));
			g.setColour(FXCOL(DefaultText));
			g.drawFittedText(text, PAD, 168, W - PAD * 2, 20, Justification::centred, 1, 0.8f);

			// name field
			const auto field = juce::Rectangle<float>((float)PAD, 208.0f, (float)(W - PAD * 2), 58.0f).withSizeKeepingCentre((float)(W - PAD * 2), 50.0f);
			g.setColour(FXCOL(WidgetBackground));
			g.fillRoundedRectangle(field, 12.0f);
			g.setColour(editor_.hasKeyboardFocus(true) ? statusColour() : FXCOL(Outline).brighter(0.2f));
			g.drawRoundedRectangle(field.reduced(0.5f), 12.0f, editor_.hasKeyboardFocus(true) ? 1.6f : 1.0f);
		}

	private:
		enum class State { Empty, Valid, Duplicate };

		static constexpr int W = 460, H = 360, PAD = 28;

		State state() const
		{
			const String name = getName();
			if (name.isEmpty())
				return State::Empty;
			auto& model = FxModel::getModel();
			for (int i = 0; i < model.getPresetCount(); i++)
				if (model.getPreset(i).name.equalsIgnoreCase(name))
					return State::Duplicate;
			return State::Valid;
		}

		Colour statusColour() const
		{
			switch (state())
			{
			case State::Valid: return FXCOL(SliderTrack);
			case State::Duplicate: return FXCOL(InvalidTextBorder);
			default: return FXCOL(Outline).brighter(0.3f);
			}
		}

		void refreshStatus()
		{
			const auto st = state();
			status_.setText(st == State::Valid ? TRANS("Ready to save")
				: st == State::Duplicate ? TRANS("A preset with that name already exists")
				: TRANS("Type a name (up to 64 characters)"), NotificationType::dontSendNotification);
			status_.setColour(Label::textColourId, st == State::Valid ? FXCOL(SliderTrack) : st == State::Duplicate ? FXCOL(InvalidTextBorder) : FXCOL(HintText));
			ok_.setEnabled(st == State::Valid);
			ok_.setAlpha(st == State::Valid ? 1.0f : 0.4f);
			repaint();
		}

		void accept()
		{
			if (state() == State::Valid)
				done_(true);
		}

		void textEditorTextChanged(TextEditor&) override { refreshStatus(); }
		void textEditorFocusLost(TextEditor&) override { repaint(); }

		Mode mode_;
		std::function<void(bool)> done_;
		Label heading_, status_;
		TextEditor editor_;
		PresetNameInputFilter filter_;
		TextButton cancel_, ok_;
	};

	Content content_;
	bool accepted_ = false;
};

//==============================================================================
FxMainWindow::FxMainWindow() : power_button_(L"powerButton"), menu_button_(L"menuButton", DrawableButton::ButtonStyle::ImageFitted), resize_button_(L"resizeButton", DrawableButton::ButtonStyle::ImageFitted), donate_button_(L"donateButton", DrawableButton::ButtonStyle::ImageFitted), minimize_button_(L"minimizeButton", DrawableButton::ButtonStyle::ImageFitted)
{
	setName("Nubetalo Sound");
	setOpaque(false);
    enableShadow(false);

	setWantsKeyboardFocus(true);

	FxModel::getModel().addListener(this);

	power_button_.setPowerState(FxModel::getModel().getPowerState());

	power_button_.setMouseCursor(MouseCursor::PointingHandCursor);
	power_button_.setSize(46, 28);
	power_button_.setImageWidth(46);
	power_button_.setHelpText(TRANS("Power Button"));
	power_button_.setWantsKeyboardFocus(true);
	power_button_.addListener(this);

	menu_button_.setMouseCursor(MouseCursor::PointingHandCursor);
	menu_button_.setSize(BUTTON_WIDTH, BUTTON_WIDTH);
	menu_button_.setHelpText(TRANS("Menu Button"));;
	menu_button_.setWantsKeyboardFocus(true);
	menu_button_.addListener(this);

	resize_button_.setMouseCursor(MouseCursor::PointingHandCursor);	
	resize_button_.setHelpText(TRANS("Resize Button"));
	resize_button_.setWantsKeyboardFocus(true);
	resize_button_.addListener(this);	

	donate_button_.setMouseCursor(MouseCursor::PointingHandCursor);
	donate_button_.setSize(BUTTON_WIDTH + 2, BUTTON_WIDTH + 6);
	donate_button_.setHelpText(TRANS("Donate"));
	donate_button_.setTooltip(TRANS("Donate"));
	donate_button_.setWantsKeyboardFocus(true);
	donate_button_.onClick = []() { FxDonate::open(); };

	minimize_button_.setMouseCursor(MouseCursor::PointingHandCursor);
	minimize_button_.setSize(BUTTON_WIDTH, BUTTON_WIDTH);
	minimize_button_.setHelpText(TRANS("Minimize Button"));
	minimize_button_.setWantsKeyboardFocus(true);
	minimize_button_.addListener(this);

	help_bubble_.addToDesktop(0);
	help_bubble_.setAlwaysOnTop(true);

	setLookAndFeel();

	addToolbarButton(&power_button_, false);
	addToolbarButton(&menu_button_, false);
	addToolbarButton(&resize_button_);
	addToolbarButton(&minimize_button_);

#ifndef NUBETALO_RELEASE
	// Developer aid: FXSOUND_MENU=<file.png> lays the menu rows out in a panel, saves a picture of it and quits.
	auto menu_snapshot = SystemStats::getEnvironmentVariable("FXSOUND_MENU", "");
	if (menu_snapshot.isNotEmpty())
	{
		Component::SafePointer<FxMainWindow> self(this);
		Timer::callAfterDelay(1500, [self, menu_snapshot]() {
			if (self == nullptr) return;
			struct Panel : Component { void paint(Graphics& g) override { getLookAndFeel().drawPopupMenuBackground(g, getWidth(), getHeight()); } };
			auto menu = self->buildMenu();
			Panel panel;
			int y = 8;
			PopupMenu::MenuItemIterator it(menu);
			while (it.next())
			{
				auto& item = it.getItem();
				if (auto* c = item.customComponent.get())
				{
					int w = 0, h = 0;
					c->getIdealSize(w, h);
					c->setEnabled(item.isEnabled);
					c->setBounds(8, y, w, h);
					panel.addAndMakeVisible(c);
					y += h;
				}
			}
			panel.setSize(356, y + 8);
			auto image = panel.createComponentSnapshot(panel.getLocalBounds(), true, 1.0f);
			File file(menu_snapshot);
			file.deleteFile();
			FileOutputStream stream(file);
			if (stream.openedOk())
				PNGImageFormat().writeImageToStream(image, stream);
			panel.removeAllChildren();
			JUCEApplication::quit();
			});
	}

	// Developer aid: FXSOUND_NAMEDLG=<file.png> opens the preset-name dialog, saves a picture of it and quits.
	auto name_snapshot = SystemStats::getEnvironmentVariable("FXSOUND_NAMEDLG", "");
	if (name_snapshot.isNotEmpty())
	{
		Component::SafePointer<FxMainWindow> self(this);
		Timer::callAfterDelay(1500, [self, name_snapshot]() {
			if (self == nullptr) return;
			FxPresetNameDialog dialog(FxPresetNameDialog::Mode::Save);
			Timer::callAfterDelay(1000, [&dialog, name_snapshot]() {
				auto image = dialog.createComponentSnapshot(dialog.getLocalBounds(), true, 1.0f);
				File file(name_snapshot);
				file.deleteFile();
				FileOutputStream stream(file);
				if (stream.openedOk())
					PNGImageFormat().writeImageToStream(image, stream);
				JUCEApplication::quit();
				});
			dialog.runModalLoop();
			});
	}

	// Developer aid: FXSOUND_PRESETIO=export|import exercises the preset export/import code, logs the result and quits.
	auto preset_io = SystemStats::getEnvironmentVariable("FXSOUND_PRESETIO", "");
	if (preset_io.isNotEmpty())
	{
		Component::SafePointer<FxMainWindow> self(this);
		Timer::callAfterDelay(2500, [self, preset_io]() {
			if (self == nullptr) return;
			auto& controller = FxController::getInstance();
			auto& model = FxModel::getModel();
			String log;
			auto export_dir = File(File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "\\Nubetalo Sound\\Presets\\Export");
			if (preset_io == "export")
			{
				Array<FxModel::Preset> user;
				for (int i = 0; i < model.getPresetCount(); i++)
					if (model.getPreset(i).type == FxModel::PresetType::UserPreset)
						user.add(model.getPreset(i));
				const bool ok = controller.exportPresets(user);
				log << "export: presets de usuario=" << user.size() << " exportado=" << (int)ok << "\n";
			}
			else
			{
				Array<File> files = export_dir.findChildFiles(File::findFiles, false, "*.fac");
				StringArray imported, skipped;
				const bool ok = controller.importPresets(files, imported, skipped);
				log << "import: archivos=" << files.size() << " ok=" << (int)ok << " importados=" << imported.joinIntoString(",") << " omitidos=" << skipped.joinIntoString(",") << "\n";
			}
			File::getSpecialLocation(File::tempDirectory).getChildFile("nubetalo_presetio.log").appendText(log);
			JUCEApplication::quit();
			});
	}

	// Developer aid: FXSOUND_SETTINGS=<file.png> opens the settings dialog, saves a picture of it and quits.
	auto settings_snapshot = SystemStats::getEnvironmentVariable("FXSOUND_SETTINGS", "");
	if (settings_snapshot.isNotEmpty())
	{
		Component::SafePointer<FxMainWindow> self(this);
		Timer::callAfterDelay(1500, [self, settings_snapshot]() {
			if (self == nullptr) return;
			FxSettingsDialog dialog;
			dialog.showTab(SystemStats::getEnvironmentVariable("FXSOUND_TAB", "0").getIntValue());
			Timer::callAfterDelay(1200, [&dialog, settings_snapshot]() {
				auto image = dialog.createComponentSnapshot(dialog.getLocalBounds(), true, 1.0f);
				File file(settings_snapshot);
				file.deleteFile();
				FileOutputStream stream(file);
				if (stream.openedOk())
					PNGImageFormat().writeImageToStream(image, stream);
				JUCEApplication::quit();
				});
			dialog.runModalLoop();
			});
	}

	// Developer aid: FXSOUND_SNAPSHOT=<file.png> saves a picture of this window and quits.
	auto snapshot_path = SystemStats::getEnvironmentVariable("FXSOUND_SNAPSHOT", "");
	if (snapshot_path.isNotEmpty())
	{
		Component::SafePointer<FxMainWindow> self(this);
		Timer::callAfterDelay(3000, [self, snapshot_path]() {
			if (self != nullptr && SystemStats::getEnvironmentVariable("FXSOUND_VIEW", "") == "2")
				self->showProView();
			if (self != nullptr && SystemStats::getEnvironmentVariable("FXSOUND_SHOW", "") == "1")
			{
				self->show();
				self->startVisualizer();
			}
			Timer::callAfterDelay(800, [self, snapshot_path]() {
			if (self != nullptr)
			{
				auto image = self->createComponentSnapshot(self->getLocalBounds(), true, 1.0f);
				File file(snapshot_path);
				file.deleteFile();
				FileOutputStream stream(file);
				if (stream.openedOk())
				{
					PNGImageFormat().writeImageToStream(image, stream);
				}
			}
			JUCEApplication::quit();
			});
			});
	}
#endif // NUBETALO_RELEASE
}

FxMainWindow::~FxMainWindow()
{
	help_bubble_.removeFromDesktop();
	FxModel::getModel().removeListener(this);
}

void FxMainWindow::show()
{
	if (!isVisible())
	{
		setVisible(true);
	}

	addToDesktop(ComponentPeer::windowAppearsOnTaskbar);
	toFront(true);

	// Bring window to the top
	auto* peer = getPeer();
	if (peer)
	{
		HWND hwnd = (HWND)peer->getNativeHandle();
		if (IsIconic(hwnd)) // If minimized, restore first
			ShowWindow(hwnd, SW_RESTORE);

		SetForegroundWindow(hwnd); // Bring to front
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}

void FxMainWindow::showLiteView()
{
	setContent(&lite_view_);
    setResizeImage();
	setAlwaysOnTop(FxController::getInstance().isAlwaysOnTop());

	auto bounds = getBounds();
	auto pos = FxController::getInstance().getSystemTrayWindowPosition(bounds.getWidth(), bounds.getHeight());
	bounds.setPosition(pos);
	setBounds(bounds);
}

void FxMainWindow::showProView()
{
    pro_view_.update();
	setContent(&pro_view_);
    setResizeImage();
	setAlwaysOnTop(FxController::getInstance().isAlwaysOnTop());
	
	int x = 0;
	int y = 0;

	auto bounds = getBounds();
	auto desktop_bounds = Desktop::getInstance().getDisplays().getTotalBounds(true);

	FxController::getInstance().getWindowPosition(x, y);
	if (x == 0 && y == 0)
	{
		centreWithSize(getWidth(), getHeight());		
	}
	else 
	{
		bounds.setPosition(Point<int>(x, y));
		if (desktop_bounds.contains(bounds))
		{
			setBounds(bounds);
		}
		else
		{
			centreWithSize(getWidth(), getHeight());
		}
	}	
}

void FxMainWindow::update()
{
	pro_view_.update();
}

void FxMainWindow::startVisualizer()
{
    pro_view_.startVisualizer();
}

void FxMainWindow::pauseVisualizer()
{
    pro_view_.pauseVisualizer();
}

void FxMainWindow::setLookAndFeel()
{
	auto& theme = dynamic_cast<FxTheme&>(LookAndFeel::getDefaultLookAndFeel());

	menu_image_ = Drawable::createFromImageData(FXIMAGE(MenuButton), FXIMAGESIZE(MenuButton));
	menu_hover_image_ = Drawable::createFromImageData(FXIMAGE(MenuButtonHover), FXIMAGESIZE(MenuButtonHover));
	menu_button_.setImages(menu_image_.get(), menu_hover_image_.get());

	donate_image_ = Drawable::createFromImageData(FXIMAGE(DonateButton), FXIMAGESIZE(DonateButton));
	donate_hover_image_ = Drawable::createFromImageData(FXIMAGE(DonateButtonHover), FXIMAGESIZE(DonateButtonHover));
	donate_button_.setImages(donate_image_.get(), donate_hover_image_.get());

	minimize_image_ = Drawable::createFromImageData(FXIMAGE(MinimizeWindowButton), FXIMAGESIZE(MinimizeWindowButton));
	minimize_hover_image_ = Drawable::createFromImageData(FXIMAGE(MinimizeWindowButtonHover), FXIMAGESIZE(MinimizeWindowButtonHover));
	minimize_button_.setImages(minimize_image_.get(), minimize_hover_image_.get());

	help_bubble_.setColour(BubbleComponent::ColourIds::backgroundColourId, Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
	help_bubble_.setColour(BubbleComponent::ColourIds::outlineColourId, theme.findColour(TextEditor::textColourId));

	pro_view_.setLookAndFeel();

	setResizeImage();
}

void FxMainWindow::setResizeImage()
{
	if (FxController::getInstance().getCurrentView() == ViewType::Pro)
	{
		resize_image_ = Drawable::createFromImageData(FXIMAGE(MinimizeButton), FXIMAGESIZE(MinimizeButton));
		resize_hover_image_ = Drawable::createFromImageData(FXIMAGE(MinimizeButtonHover), FXIMAGESIZE(MinimizeButtonHover));
		resize_button_.setSize(BUTTON_WIDTH, BUTTON_WIDTH);
	}
	else
	{
		resize_image_ = Drawable::createFromImageData(FXIMAGE(MaximizeButton), FXIMAGESIZE(MaximizeButton));
		resize_hover_image_ = Drawable::createFromImageData(FXIMAGE(MaximizeButtonHover), FXIMAGESIZE(MaximizeButtonHover));
		resize_button_.setSize(BUTTON_WIDTH, BUTTON_WIDTH); 	
	}

	resize_button_.setImages(resize_image_.get(), resize_hover_image_.get());
}

void FxMainWindow::setIcon(bool power, bool processing)
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	HWND hWnd = (HWND)getWindowHandle();

	HICON curr_icon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0);
	HICON icon = NULL;

	if (power)
	{
		if (processing)
		{
			if (FxTheme::getThemeMode() == FxThemeMode::Dark)
				icon = LoadIcon(hInst, L"IDI_LOGO_RED");
			else
				icon = LoadIcon(hInst, L"IDI_LOGO_BLUE");
		}
		else
		{
			icon = LoadIcon(hInst, L"IDI_LOGO_WHITE");
		}
	}
	else
	{
		icon = LoadIcon(hInst, L"IDI_LOGO_GRAY");
	}

	if (icon != NULL)
	{
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
	}

	if (curr_icon != NULL)
	{
		DestroyIcon(curr_icon);
	}
}

void FxMainWindow::enablePowerButton(bool enable)
{
	if (power_button_.isEnabled() != enable)
	{
		power_button_.setEnabled(enable);

		if (!enable)
		{
			power_button_.setTooltip(TRANS("Audio enhancements are not available over Remote Desktop"));
		}
		else
		{
			power_button_.setTooltip("");
		}
	}
}

bool FxMainWindow::keyPressed(const KeyPress& key)
{
	if (key.getModifiers().isAltDown() && key.getKeyCode() == juce::KeyPress::returnKey)
	{
		FxController::getInstance().setMenuClicked(true);
		showMenu();
		return true;
	}

	return Component::keyPressed(key);
}

void FxMainWindow::visibilityChanged()
{
	auto* peer = getPeer();
	if (peer)
	{
		HWND hwnd = (HWND)peer->getNativeHandle();
		LONG style = GetWindowLong(hwnd, GWL_STYLE);
		if ((style & WS_MINIMIZEBOX) == 0)
		{
			SetWindowLong(hwnd, GWL_STYLE, style | WS_MINIMIZEBOX);
		}
	}
}

// One row of the settings-style menu: rounded group cards, a coloured icon tile, chevron or switch on the right.
class FxMenuRow : public PopupMenu::CustomComponent
{
public:
	enum class Pos { Single, First, Middle, Last };
	enum class Kind { Plain, Chevron, Toggle };

	FxMenuRow(const String& text, const char* glyph, Colour tile, Pos pos, Kind kind, bool toggle_on = false, const String& subtitle = String())
		: PopupMenu::CustomComponent(true), text_(text), subtitle_(subtitle), tile_(tile), pos_(pos), kind_(kind), on_(toggle_on)
	{
		String svg = String("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24'><path d='") + glyph +
			"' fill='none' stroke='white' stroke-width='1.9' stroke-linecap='round' stroke-linejoin='round'/></svg>";
		glyph_ = Drawable::createFromImageData(svg.toRawUTF8(), svg.getNumBytesAsUTF8());
	}

	void getIdealSize(int& w, int& h) override
	{
		w = 340;
		h = (subtitle_.isEmpty() ? ROW_H : ROW_H + 10) + (hasGapAbove() ? GAP : 0);
	}

	void paint(Graphics& g) override
	{
		auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
		auto area = getLocalBounds().toFloat().reduced(2.0f, 0.0f);
		if (hasGapAbove())
			area.removeFromTop((float)GAP);

		const bool enabled = isEnabled();
		const bool round_top = (pos_ == Pos::Single || pos_ == Pos::First);
		const bool round_bottom = (pos_ == Pos::Single || pos_ == Pos::Last);

		Path card;
		card.addRoundedRectangle(area.getX(), area.getY(), area.getWidth(), area.getHeight(), 13.0f, 13.0f, round_top, round_top, round_bottom, round_bottom);
		g.setColour(isItemHighlighted() && enabled ? FXCOL(MenuHighlightBackground) : FXCOL(ControlBackground).brighter(0.03f));
		g.fillPath(card);

		if (pos_ == Pos::First || pos_ == Pos::Middle)
		{
			g.setColour(FXCOL(Outline));
			g.fillRect(area.getX() + 54.0f, area.getBottom() - 1.0f, area.getWidth() - 54.0f - 14.0f, 1.0f);
		}

		// icon tile
		auto tile = juce::Rectangle<float>(area.getX() + 14.0f, area.getCentreY() - 14.0f, 28.0f, 28.0f);
		g.setColour(enabled ? tile_ : FXCOL(MenuBackground));
		g.fillRoundedRectangle(tile, 8.0f);
		if (glyph_ != nullptr)
			glyph_->drawWithin(g, tile.reduced(5.0f), RectanglePlacement::centred, enabled ? 1.0f : 0.4f);

		// text
		g.setColour(enabled ? FXCOL(HighlightedText) : FXCOL(HintText));
		g.setFont(theme.getNormalFont().withHeight(16.0f));
		if (subtitle_.isEmpty())
			g.drawText(text_, area.withTrimmedLeft(56.0f).withTrimmedRight(70.0f), Justification::centredLeft, true);
		else
		{
			auto text_area = area.withTrimmedLeft(56.0f).withTrimmedRight(70.0f);
			g.drawText(text_, text_area.withTrimmedTop(7.0f).withHeight(22.0f), Justification::centredLeft, true);
			g.setColour(FXCOL(HintText));
			g.setFont(theme.getNormalFont().withHeight(12.0f));
			g.drawText(subtitle_, text_area.withTrimmedTop(29.0f).withHeight(16.0f), Justification::centredLeft, true);
		}

		// right side
		if (kind_ == Kind::Chevron)
		{
			Path p;
			const float cx = area.getRight() - 20.0f, cy = area.getCentreY();
			p.startNewSubPath(cx - 2.5f, cy - 5.0f);
			p.lineTo(cx + 2.5f, cy);
			p.lineTo(cx - 2.5f, cy + 5.0f);
			g.setColour(FXCOL(HintText));
			g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::curved, PathStrokeType::rounded));
		}
		else if (kind_ == Kind::Toggle)
		{
			auto sw = juce::Rectangle<float>(area.getRight() - 14.0f - 42.0f, area.getCentreY() - 12.0f, 42.0f, 24.0f);
			g.setColour(on_ ? FXCOL(SliderTrack) : FXCOL(MenuBackground));
			g.fillRoundedRectangle(sw, 12.0f);
			g.setColour(Colour(0xffeaf2ff));
			g.fillEllipse(sw.getX() + (on_ ? sw.getWidth() - 22.0f : 2.0f), sw.getY() + 2.0f, 20.0f, 20.0f);
		}
	}

private:
	static constexpr int ROW_H = 46;
	static constexpr int GAP = 12;
	bool hasGapAbove() const { return pos_ == Pos::Single || pos_ == Pos::First; }

	String text_, subtitle_;
	Colour tile_;
	Pos pos_;
	Kind kind_;
	bool on_;
	std::unique_ptr<Drawable> glyph_;
};

void FxMainWindow::showMenu()
{
	buildMenu().showAt(&menu_button_);
}

PopupMenu FxMainWindow::buildMenu()
{
	auto settingsClicked = [this]() {

		FxSettingsDialog settings_dialog;
		settings_dialog.runModalLoop();
		FxController::getInstance().refreshOutputList();
		pro_view_.refreshGain();
		lite_view_.refreshGain();
	};

	auto overwriteClicked = [this]() {
		FxController::getInstance().savePreset();
	};

	auto undoClicked = []() {
		FxController::getInstance().undoPreset();
	};

	auto deleteClicked = []() {
		FxController::getInstance().deletePreset();
	};

	auto exportClicked = [this]() {
		FxPresetExportDialog preset_export_dialog;
		preset_export_dialog.runModalLoop();
	};

	auto importClicked = [this]() {
		FxPresetImportDialog preset_import_dialog;
		preset_import_dialog.runModalLoop();
	};

	auto donateClicked = []() { FxDonate::open(); };

	auto darkModeClicked = [this]() {
		FxController::getInstance().setThemeMode(FxThemeMode::Dark);
	};

	auto lightModeClicked = [this]() {
		FxController::getInstance().setThemeMode(FxThemeMode::Light);
	};

	auto alwaysOnTopClicked = [this]() {
		auto& controller = FxController::getInstance();
		controller.setAlwaysOnTop(!controller.isAlwaysOnTop());
		};

	PopupMenu popup_menu;

	auto saveNewClicked = []() {
		FxPresetNameDialog dialog(FxPresetNameDialog::Mode::Save);
		if (dialog.runModalLoop() == 1)
			FxController::getInstance().savePreset(dialog.getName());
	};
	auto renameClicked = []() {
		FxPresetNameDialog dialog(FxPresetNameDialog::Mode::Rename);
		if (dialog.runModalLoop() == 1)
			FxController::getInstance().renamePreset(dialog.getName());
	};

	bool user_preset = false;
	String overwrite_menu_name = TRANS("Overwrite Existing Preset");

	auto& model = FxModel::getModel();
	auto preset = model.getPreset(model.getSelectedPreset());
	if (preset.type == FxModel::PresetType::UserPreset)
		user_preset = true;
	if (model.isPresetModified() && user_preset)
		overwrite_menu_name = overwrite_menu_name + L" - " + preset.name;

	const auto power_state = model.getPowerState();

	using Pos = FxMenuRow::Pos;
	using Kind = FxMenuRow::Kind;
	const Colour blue(0xff2f7bff), deep(0xff2563c9), light(0xff5aa0ff), red(0xffe5484d);

	auto add_row = [&popup_menu](const String& text, const char* glyph, Colour tile, Pos pos, Kind kind, bool enabled,
								 std::function<void()> action, const PopupMenu* sub = nullptr, bool toggle_on = false, const String& subtitle = String()) {
		PopupMenu::Item item(text);
		item.isEnabled = enabled;
		item.action = action;
		if (sub != nullptr)
			item.subMenu.reset(new PopupMenu(*sub));
		item.customComponent = new FxMenuRow(text, glyph, tile, pos, kind, toggle_on, subtitle);
		popup_menu.addItem(std::move(item));
	};

	// Group 1
	add_row(TRANS("Settings"), "M10.08 4.85L10.50 2.52L13.50 2.52L13.92 4.85L15.70 5.59L17.64 4.23L19.77 6.36L18.41 8.30L19.15 10.08L21.48 10.50L21.48 13.50L19.15 13.92L18.41 15.70L19.77 17.64L17.64 19.77L15.70 18.41L13.92 19.15L13.50 21.48L10.50 21.48L10.08 19.15L8.30 18.41L6.36 19.77L4.23 17.64L5.59 15.70L4.85 13.92L2.52 13.50L2.52 10.50L4.85 10.08L5.59 8.30L4.23 6.36L6.36 4.23L8.30 5.59ZM8.8 12.0a3.2 3.2 0 1 0 6.4 0a3.2 3.2 0 1 0 -6.4 0", blue, Pos::Single, Kind::Chevron, true, settingsClicked);

	// Group 2: presets
	add_row(TRANS("Save New Preset"), "M12 6v12M6 12h12", deep, Pos::First, Kind::Plain,
			model.isPresetModified() && model.getUserPresetCount() < FxController::getInstance().getMaxUserPresets() && power_state, saveNewClicked);
	add_row(overwrite_menu_name, "M12 4v10M8 10l4 4 4-4M5 19h14", deep, Pos::Middle, Kind::Plain,
			model.isPresetModified() && user_preset && power_state, overwriteClicked);
	add_row(TRANS("Undo Preset Changes"), "M9 7L5 11l4 4M5 11h9a4 4 0 0 1 0 8h-3", deep, Pos::Middle, Kind::Plain,
			model.isPresetModified() && power_state, undoClicked);
	add_row(TRANS("Rename Preset"), "M5 19l1-4 9-9 3 3-9 9-4 1zM13.5 7.5l3 3", deep, Pos::Middle, Kind::Plain,
			!model.isPresetModified() && user_preset && power_state, renameClicked);
	add_row(TRANS("Delete Preset"), "M6 7h12M9 7V5h6v2M8 7l1 12h6l1-12", red, Pos::Last, Kind::Plain,
			user_preset && power_state, deleteClicked);

	// Group 3: share
	add_row(TRANS("Export Presets"), "M12 15V5M8 9l4-4 4 4M5 19h14", light, Pos::First, Kind::Plain,
			!model.isPresetModified() && power_state, exportClicked);
	add_row(TRANS("Import Presets"), "M12 5v10M8 11l4 4 4-4M5 19h14", light, Pos::Last, Kind::Plain,
			!model.isPresetModified() && power_state, importClicked);

	// Group 4: window
	const bool donations = FxDonate::enabled();
	add_row(TRANS("Always On Top"), "M9 4h6l-1 6 3 3H7l3-3-1-6zM12 13v7", blue, donations ? Pos::First : Pos::Single, Kind::Toggle, true,
			alwaysOnTopClicked, nullptr, isAlwaysOnTop(), TRANS("Keeps this window above all others"));
	if (donations)
		add_row(TRANS("Donate"), "M12 19s-6.5-4-6.5-8.5A3.5 3.5 0 0 1 12 8.6a3.5 3.5 0 0 1 6.5 1.9C18.5 15 12 19 12 19z", red, Pos::Last, Kind::Plain, true, donateClicked);

	return popup_menu;
}

void FxMainWindow::buttonClicked(Button* button)
{
	if (button == &power_button_)
	{
		auto power_state = !FxModel::getModel().getPowerState();

		FxController::getInstance().setPowerState(power_state);

		power_button_.setPowerState(FxModel::getModel().getPowerState());

		repaint();
	}
	else if (button == &menu_button_)
	{
		FxController::getInstance().setMenuClicked(true);
		showMenu();
	}
	else if (button == &resize_button_)
	{
		FxController::getInstance().switchView();
		setResizeImage();
	}
	else if (button == &minimize_button_)
	{
		if (isOnDesktop())
		{
			ShowWindow((HWND)getWindowHandle(), SW_MINIMIZE);
		}
	}
}

void FxMainWindow::mouseEnter(const MouseEvent&)
{
	if (menu_button_.isMouseOver(true) && !FxModel::getModel().isMenuClicked())
	{
		auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());

		AttributedString text(TRANS("Click here to save new presets, overwrite old ones, or reset your settings."));
		text.setColour(theme.findColour(TextEditor::textColourId));
		text.setJustification(Justification::centred);
		text.setFont(theme.getSmallFont());

		help_bubble_.showAt(&menu_button_, text, 0, true, false);
	}
}

void FxMainWindow::modelChanged(FxModel::Event)
{
	power_button_.setPowerState(FxModel::getModel().getPowerState());
}

void FxMainWindow::userTriedToCloseWindow()
{
	FxController::getInstance().hideMainWindow();
}

void FxMainWindow::closeButtonPressed()
{
	FxController::getInstance().hideMainWindow();
}

void FxMainWindow::moved()
{
	if (FxController::getInstance().getCurrentView() == ViewType::Pro)
	{
		auto bounds = getBounds();
		auto desktop_bounds = Desktop::getInstance().getDisplays().getTotalBounds(true);
		if (desktop_bounds.contains(bounds))
		{
			// Save window position only if the whole window is visible on the desktop
			FxController::getInstance().saveWindowPosition(bounds.getX(), bounds.getY());
		}		
	}
}

void FxMainWindow::lookAndFeelChanged()
{
	setLookAndFeel();
	repaint();
}