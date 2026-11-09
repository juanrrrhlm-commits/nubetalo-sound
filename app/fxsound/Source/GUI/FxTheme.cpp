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
#include "FxTheme.h"

const uint32 FxTheme::theme_colors_[FxThemeMode::NumModes][FxColor::NumColors] =
{ { 0x07090d, 0x0b0e14, 0x10141c, 0x1a2230, 0xaeb8c8, 0x000000, 0xeaf2ff, 0x0a0d13, 0xe6eefc,
	0x05070a, 0x2f7bff, 0x5aa0ff, 0x8a95a8, 0x2f7bff, 0xe5484d, 0x0d1118, 0x2f7bff, 0x6cb0ff,
	0x2f7bff, 0x6cb0ff, 0x5aa0ff, 0x163a73, 0xe6eefc, 0x1a2a45, 0x090c12, 0x2a3547, 0x2f7bff },

  { 0x07090d, 0x0b0e14, 0x10141c, 0x1a2230, 0xaeb8c8, 0x000000, 0xeaf2ff, 0x0a0d13, 0xe6eefc,
	0x05070a, 0x2f7bff, 0x5aa0ff, 0x8a95a8, 0x2f7bff, 0xe5484d, 0x0d1118, 0x2f7bff, 0x6cb0ff,
	0x2f7bff, 0x6cb0ff, 0x5aa0ff, 0x163a73, 0xe6eefc, 0x1a2a45, 0x090c12, 0x2a3547, 0x2f7bff } };

const char* FxTheme::theme_images_[FxThemeMode::NumModes][FxImage::NumImages] =
{ { BinaryData::logowhite_svg, BinaryData::logored_svg, BinaryData::FxSound_White_Bars_svg,
	BinaryData::power_on_svg, BinaryData::power_off_svg, BinaryData::donate_svg, BinaryData::donate_hover_svg, BinaryData::menu_svg, BinaryData::menu_hover_svg,
	BinaryData::minimize_svg, BinaryData::minimize_hover_svg, BinaryData::maximize_svg, BinaryData::maximize_hover_svg, BinaryData::min_window_svg, BinaryData::min_window_hover_svg,
    BinaryData::flip_white_svg, BinaryData::flip_svg, BinaryData::restore_defaults_white_svg, BinaryData::restore_defaults_svg, BinaryData::remove_svg,
	BinaryData::arrow_next_svg, BinaryData::arrow_next_bw_svg, BinaryData::arrow_prev_svg, BinaryData::arrow_prev_bw_svg, BinaryData::arrow_up_svg, BinaryData::arrow_up_white_svg, BinaryData::arrow_down_svg,  BinaryData::arrow_down_white_svg, BinaryData::dropdown_arrow_bw_svg, BinaryData::dropdown_arrow_hover_svg,
	BinaryData::Slider_Thumb_svg, BinaryData::Slider_Thumb_bw_svg },

  { BinaryData::logowhite_svg, BinaryData::logored_svg, BinaryData::FxSound_White_Bars_svg,
	BinaryData::power_on_svg, BinaryData::power_off_svg, BinaryData::donate_svg, BinaryData::donate_hover_svg, BinaryData::menu_svg, BinaryData::menu_hover_svg,
	BinaryData::minimize_svg, BinaryData::minimize_hover_svg, BinaryData::maximize_svg, BinaryData::maximize_hover_svg, BinaryData::min_window_svg, BinaryData::min_window_hover_svg,
    BinaryData::flip_white_svg, BinaryData::flip_svg, BinaryData::restore_defaults_white_svg, BinaryData::restore_defaults_svg, BinaryData::remove_svg,
	BinaryData::arrow_next_svg, BinaryData::arrow_next_bw_svg, BinaryData::arrow_prev_svg, BinaryData::arrow_prev_bw_svg, BinaryData::arrow_up_svg, BinaryData::arrow_up_white_svg, BinaryData::arrow_down_svg,  BinaryData::arrow_down_white_svg, BinaryData::dropdown_arrow_bw_svg, BinaryData::dropdown_arrow_hover_svg,
	BinaryData::Slider_Thumb_svg, BinaryData::Slider_Thumb_bw_svg } };

const int FxTheme::theme_image_sizes_[FxThemeMode::NumModes][FxImage::NumImages] =
{ {BinaryData::logowhite_svgSize, BinaryData::logored_svgSize, BinaryData::FxSound_White_Bars_svgSize,
   BinaryData::power_on_svgSize, BinaryData::power_off_svgSize, BinaryData::donate_svgSize, BinaryData::donate_hover_svgSize, BinaryData::menu_svgSize, BinaryData::menu_hover_svgSize,
   BinaryData::minimize_svgSize, BinaryData::minimize_hover_svgSize, BinaryData::maximize_svgSize, BinaryData::maximize_hover_svgSize, BinaryData::min_window_svgSize, BinaryData::min_window_hover_svgSize,
   BinaryData::flip_white_svgSize, BinaryData::flip_svgSize, BinaryData::restore_defaults_white_svgSize, BinaryData::restore_defaults_svgSize, BinaryData::remove_svgSize,
   BinaryData::arrow_next_svgSize, BinaryData::arrow_next_bw_svgSize, BinaryData::arrow_prev_svgSize, BinaryData::arrow_prev_bw_svgSize, BinaryData::arrow_up_svgSize, BinaryData::arrow_up_white_svgSize, BinaryData::arrow_down_svgSize,  BinaryData::arrow_down_white_svgSize, BinaryData::dropdown_arrow_bw_svgSize, BinaryData::dropdown_arrow_hover_svgSize,
   BinaryData::Slider_Thumb_svgSize, BinaryData::Slider_Thumb_bw_svgSize },

  {BinaryData::logowhite_svgSize, BinaryData::logored_svgSize, BinaryData::FxSound_White_Bars_svgSize,
   BinaryData::power_on_svgSize, BinaryData::power_off_svgSize, BinaryData::donate_svgSize, BinaryData::donate_hover_svgSize, BinaryData::menu_svgSize, BinaryData::menu_hover_svgSize,
   BinaryData::minimize_svgSize, BinaryData::minimize_hover_svgSize, BinaryData::maximize_svgSize, BinaryData::maximize_hover_svgSize, BinaryData::min_window_svgSize, BinaryData::min_window_hover_svgSize,
   BinaryData::flip_white_svgSize, BinaryData::flip_svgSize, BinaryData::restore_defaults_white_svgSize, BinaryData::restore_defaults_svgSize, BinaryData::remove_svgSize,
   BinaryData::arrow_next_svgSize, BinaryData::arrow_next_bw_svgSize, BinaryData::arrow_prev_svgSize, BinaryData::arrow_prev_bw_svgSize, BinaryData::arrow_up_svgSize, BinaryData::arrow_up_white_svgSize, BinaryData::arrow_down_svgSize,  BinaryData::arrow_down_white_svgSize, BinaryData::dropdown_arrow_bw_svgSize, BinaryData::dropdown_arrow_hover_svgSize,
   BinaryData::Slider_Thumb_svgSize, BinaryData::Slider_Thumb_bw_svgSize } };

FxThemeMode FxTheme::theme_mode_ = FxThemeMode::Dark;

//==============================================================================
FxTheme::FxTheme() : LookAndFeel_V4()
{
	init();
}

void FxTheme::init()
{
	setColourScheme(getFxColourScheme());

	setColour(ComboBox::ColourIds::arrowColourId, Colour(FXCOLOR(ImageButton)).withAlpha(1.0f));
	setColour(ComboBox::ColourIds::backgroundColourId, Colour(FXCOLOR(ComboBoxBackground)).withAlpha(1.0f));
	setColour(ComboBox::ColourIds::outlineColourId, Colour(FXCOLOR(ComboBoxBackground)).withAlpha(1.0f));
	setColour(ComboBox::ColourIds::focusedOutlineColourId, Colour(FXCOLOR(SliderHighlight)).withAlpha(0.2f));
	setColour(ComboBox::ColourIds::textColourId, Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
	setColour(TextEditor::ColourIds::backgroundColourId, Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
	setColour(TextEditor::ColourIds::outlineColourId, Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
	setColour(TextEditor::ColourIds::focusedOutlineColourId, Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
	setColour(TextEditor::ColourIds::textColourId, Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
	setColour(TextEditor::ColourIds::highlightedTextColourId, Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
	setColour(TextButton::ColourIds::buttonColourId, Colour(FXCOLOR(TextButtonBackground)).withAlpha(1.0f));
	setColour(TextButton::ColourIds::buttonOnColourId, Colour(FXCOLOR(TextButtonBackground)).withAlpha(1.0f));
	setColour(TextButton::ColourIds::textColourOffId, Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
	setColour(TextButton::ColourIds::textColourOnId, Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
	setColour(HyperlinkButton::ColourIds::textColourId, Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
	setColour(CaretComponent::ColourIds::caretColourId, Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
	// Transparent window; drawPopupMenuBackground paints the rounded panel.
	setColour(PopupMenu::ColourIds::backgroundColourId, Colour(0x00000000));
	setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colour(FXCOLOR(ImageButton)).withAlpha(1.0f));
	setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour(FXCOLOR(SliderTrack)).withAlpha(0.2f));
	setColour(Slider::ColourIds::rotarySliderFillColourId, Colour(FXCOLOR(SliderTrack)).withAlpha(1.0f));
	setColour(ScrollBar::thumbColourId, Colour(FXCOLOR(SliderTrack)).withAlpha(1.0f));

	font_400_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansRegular_ttf, BinaryData::IBMPlexSansRegular_ttfSize);
	font_600_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansMedium_ttf, BinaryData::IBMPlexSansMedium_ttfSize);
	font_700_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansSemiBold_ttf, BinaryData::IBMPlexSansSemiBold_ttfSize);

	drop_down_arrow_ = Drawable::createFromImageData(FXIMAGE(DropDownArrowHover), FXIMAGESIZE(DropDownArrowHover));
	slider_thumb_ = Drawable::createFromImageData(FXIMAGE(SliderThumb), FXIMAGESIZE(SliderThumb));
	drop_down_arrow_grey_ = Drawable::createFromImageData(FXIMAGE(DropDownArrow), FXIMAGESIZE(DropDownArrow));
	slider_thumb_grey_ = Drawable::createFromImageData(FXIMAGE(SliderThumbBW), FXIMAGESIZE(SliderThumbBW));
}

LookAndFeel_V4::ColourScheme FxTheme::getFxColourScheme()
{
	return { Colour(FXCOLOR(WindowBackground)).withAlpha(1.0f), Colour(FXCOLOR(WidgetBackground)).withAlpha(1.0f), Colour(FXCOLOR(MenuBackground)).withAlpha(1.0f),
			 Colour(FXCOLOR(Outline)).withAlpha(1.0f), Colour(FXCOLOR(DefaultText)).withAlpha(1.0f), Colour(FXCOLOR(DefaultFill)).withAlpha(0.2f),
			 Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f), Colour(FXCOLOR(HighlightedFill)).withAlpha(1.0f), Colour(FXCOLOR(MenuText)).withAlpha(1.0f) };
}

Label* FxTheme::createComboBoxTextBox(ComboBox& box)
{
	auto label = LookAndFeel_V4::createComboBoxTextBox(box);
	label->setMouseCursor(MouseCursor::PointingHandCursor);
	box.setMouseCursor(MouseCursor::PointingHandCursor);
	return label;
}

Font FxTheme::getComboBoxFont(ComboBox& box)
{
	if (box.getHeight() <= 44)
		return Font(font_600_).withHeight(14.0f);
    else
		return Font(font_600_).withHeight(17.0f);
}

void FxTheme::positionComboBoxText(ComboBox& box, Label& label)
{
	label.setMinimumHorizontalScale(1.0);
	LookAndFeel_V4::positionComboBoxText(box, label);
	label.setBounds(label.getBounds().withX(8).withRight(box.getWidth() - 32));
}

void FxTheme::drawComboBox(Graphics& g, int width, int height, bool,
							int, int, int, int, ComboBox& box)
{
	const float corner = 8.0f;
	auto bounds = Rectangle<float>(0.0f, 0.0f, (float)width, (float)height);
	const bool enabled = box.isEnabled();

	g.setColour(FXCOL(WidgetBackground));
	g.fillRoundedRectangle(bounds, corner);

	g.setColour(box.hasKeyboardFocus(true) ? FXCOL(SliderTrack) : FXCOL(Outline).brighter(0.15f));
	g.drawRoundedRectangle(bounds.reduced(0.5f), corner, 1.0f);

	// Chevron
	const float cx = width - 18.0f;
	const float cy = height / 2.0f;
	Path chevron;
	chevron.startNewSubPath(cx - 4.0f, cy - 2.0f);
	chevron.lineTo(cx, cy + 2.0f);
	chevron.lineTo(cx + 4.0f, cy - 2.0f);
	g.setColour(enabled ? FXCOL(DefaultText) : Colour(FXCOLOR(HintText)).withAlpha(0.5f));
	g.strokePath(chevron, PathStrokeType(1.6f, PathStrokeType::curved, PathStrokeType::rounded));
}

void FxTheme::drawComboBoxTextWhenNothingSelected(Graphics& g, ComboBox& box, Label& label)
{
	g.setColour(findColour(ComboBox::textColourId).withMultipliedAlpha(0.5f));

	auto font = label.getLookAndFeel().getLabelFont(label);

	g.setFont(font);

	auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds().withX(10));

	g.drawFittedText(box.getTextWhenNothingSelected(), textArea, label.getJustificationType(),
		jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
		label.getMinimumHorizontalScale());
}

void FxTheme::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
								float sliderPos, float minSliderPos, float maxSliderPos,
								const Slider::SliderStyle style, Slider& slider)
{
	const bool enabled = slider.isEnabled();
	const Colour accent = enabled ? FXCOL(SliderTrack) : FXCOL(SliderTrack).withSaturation(0.0f).withMultipliedBrightness(0.6f);
	const Colour rail = FXCOL(Outline);
	const bool active = slider.getThumbBeingDragged() >= 0 || slider.hasKeyboardFocus(true);

	if (style == Slider::LinearVertical)
	{
		const float cx = x + width / 2.0f;
		const float top = (float)y;
		const float bottom = (float)(y + height);
		const float zero = jlimit(top, bottom, (float)slider.getPositionOfValue(0.0));
		const float r = active ? 6.5f : 5.5f;

		// Rail, then the lit segment between the 0 dB line and the thumb.
		g.setColour(rail);
		g.drawLine(cx, top, cx, bottom, 2.0f);
		g.setColour(rail.brighter(0.5f));
		g.drawLine(cx - 5.0f, zero, cx + 5.0f, zero, 1.0f);

		if (enabled)
		{
			g.setColour(accent.withAlpha(active ? 0.28f : 0.16f));
			g.drawLine(cx, zero, cx, sliderPos, 8.0f);
		}
		g.setColour(accent);
		g.drawLine(cx, zero, cx, sliderPos, 2.0f);

		// Hollow thumb: dark centre, cobalt ring.
		g.setColour(FXCOL(WindowBackground));
		g.fillEllipse(cx - r, sliderPos - r, r * 2, r * 2);
		g.setColour(accent);
		g.drawEllipse(cx - r, sliderPos - r, r * 2, r * 2, 2.0f);
	}
	else if (style == Slider::LinearHorizontal)
	{
		const float cy = y + height / 2.0f;
		const float left = (float)x;
		const float r = active ? 6.0f : 5.0f;

		g.setColour(rail);
		g.drawLine(left, cy, left + width, cy, 2.0f);

		if (enabled)
		{
			g.setColour(accent.withAlpha(active ? 0.28f : 0.16f));
			g.drawLine(left, cy, sliderPos, cy, 7.0f);
		}
		g.setColour(accent);
		g.drawLine(left, cy, sliderPos, cy, 2.0f);

		g.setColour(enabled ? FXCOL(HighlightedText) : accent);
		g.fillEllipse(sliderPos - r, cy - r, r * 2, r * 2);
	}
	else
	{
		LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
	}
}

void FxTheme::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(Slider::rotarySliderFillColourId);

    if (!slider.isEnabled())
    {
        outline = outline.withSaturation(0.0);
        fill = fill.withSaturation(0.0);
    }

    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(2);

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 5.0f;
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(outline);
    g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

    Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        toAngle,
        true);

    g.setColour(fill);
    g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
	if (slider.hasKeyboardFocus(true))
	{
		DropShadow shadow;
		shadow.colour = Colour(FXCOLOR(SliderHighlight)).withAlpha(0.1f);
		shadow.drawForPath(g, backgroundArc);
	}

    auto thumbRadius = getSliderThumbRadius(slider);
    Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
                            bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
    auto thumbX = thumbPoint.getX() - thumbRadius;
    auto thumbY = thumbPoint.getY() - thumbRadius;

    if (slider.isEnabled())
        slider_thumb_->drawWithin(g, Rectangle<float>(thumbX, thumbY, thumbRadius*2, thumbRadius*2), { RectanglePlacement::centred }, 1.0f);
    else
        slider_thumb_grey_->drawWithin(g, Rectangle<float>(thumbX, thumbY, thumbRadius * 2, thumbRadius * 2), { RectanglePlacement::centred }, 1.0f);
}

int FxTheme::getSliderThumbRadius(Slider& slider)
{
    if (slider.getSliderStyle() == Slider::Rotary)
    {
        return ROTARY_SLIDER_THUMB_RADIUS;
    }
    else
    {
        return SLIDER_THUMB_RADIUS;
    }
}

Slider::SliderLayout FxTheme::getSliderLayout(Slider& slider)
{
	auto layout = LookAndFeel_V4::getSliderLayout(slider);
	auto style = slider.getSliderStyle();

	if (style == Slider::LinearVertical)
	{
		auto y = layout.sliderBounds.getY() + (SLIDER_THUMB_RADIUS*2);
		auto height = layout.sliderBounds.getHeight() - (SLIDER_THUMB_RADIUS*2);
		layout.sliderBounds.setY(y);
		layout.sliderBounds.setHeight(height);
	}
	else if (style == Slider::LinearHorizontal)
	{
		// Sliders that show their value at the right end reserve room for it.
		int reserve = (int)slider.getProperties().getWithDefault("reserve", SLIDER_THUMB_RADIUS * 4);
		auto width = layout.sliderBounds.getWidth() - reserve;
		layout.sliderBounds.setWidth(width);
	}

	return layout;
}

void FxTheme::drawPopupMenuItem(Graphics& g, const juce::Rectangle<int>& area, bool is_separator, bool is_active,
								bool is_highlighted, bool is_ticked, bool has_submenu, const String& text,
								const String& shortcut_key_text, const Drawable* icon, const Colour* text_colour)
{
	LookAndFeel_V4::drawPopupMenuItem(g, area, is_separator, is_active, is_highlighted||is_ticked, is_ticked,
										has_submenu, text, shortcut_key_text, icon, text_colour);
	
	if (is_ticked)
	{
		g.setColour(findColour(PopupMenu::textColourId).withAlpha(1.0f));
		g.drawRect(area.toFloat());
	}
}

Font FxTheme::getPopupMenuFont()
{
	return Font(font_600_).withHeight(17.0f);
}

void FxTheme::drawPopupMenuBackground(Graphics& g, int width, int height)
{
	auto r = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(1.0f);
	g.setColour(FXCOL(WindowBackground));
	g.fillRoundedRectangle(r, 18.0f);
	g.setColour(FXCOL(Outline).brighter(0.2f));
	g.drawRoundedRectangle(r.reduced(0.5f), 18.0f, 1.0f);
}

void FxTheme::preparePopupMenuWindow(Component& new_window)
{
	new_window.setMouseCursor(MouseCursor::PointingHandCursor);
	auto children = new_window.getChildren();
	for (auto child : children)
	{
		child->setMouseCursor(MouseCursor::PointingHandCursor);
	}
}

String FxTheme::fontFamilyForLanguage(const String& l)
{
	if (l.startsWithIgnoreCase("ja")) return "Yu Gothic UI";
	if (l.startsWithIgnoreCase("ko")) return "Malgun Gothic";
	if (l.startsWithIgnoreCase("zh-TW")) return "Microsoft JhengHei UI";
	if (l.startsWithIgnoreCase("zh")) return "Microsoft YaHei UI";
	if (l.startsWithIgnoreCase("th")) return "Leelawadee UI";
	if (l.startsWithIgnoreCase("ar") || l.startsWithIgnoreCase("fa")) return "Segoe UI";
	return String();
}

void FxTheme::loadFont(String language)
{
	const auto family = fontFamilyForLanguage(language);
	if (family.isNotEmpty())
	{
		font_400_ = Typeface::createSystemTypefaceFor(Font(family, 14.0f, Font::plain));
		font_600_ = Typeface::createSystemTypefaceFor(Font(family, 14.0f, Font::plain));
		font_700_ = Typeface::createSystemTypefaceFor(Font(family, 14.0f, Font::bold));
	}
	else
	{
		font_400_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansRegular_ttf, BinaryData::IBMPlexSansRegular_ttfSize);
		font_600_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansMedium_ttf, BinaryData::IBMPlexSansMedium_ttfSize);
		font_700_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansSemiBold_ttf, BinaryData::IBMPlexSansSemiBold_ttfSize);
	}

    if (font_400_ == nullptr)
    {
        font_400_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansRegular_ttf, BinaryData::IBMPlexSansRegular_ttfSize);
    }
    if (font_600_ == nullptr)
    {
        font_600_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansMedium_ttf, BinaryData::IBMPlexSansMedium_ttfSize);
    }
    if (font_700_ == nullptr)
    {
        font_700_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansSemiBold_ttf, BinaryData::IBMPlexSansSemiBold_ttfSize);
    }

    setDefaultSansSerifTypeface(font_600_);
}

Font FxTheme::getTextButtonFont(TextButton&, int button_height)
{
	return Font(font_600_.get()).withHeight(jmin(17.0f, (float)button_height));
}

Font FxTheme::getNormalFont()
{
	return Font(font_600_).withHeight(17.0f);
}

Font FxTheme::getSmallFont()
{
	return Font(font_400_).withHeight(14.0f);
}

Font FxTheme::getTitleFont()
{
	return Font(font_700_).withHeight(17.0f);
}

Typeface::Ptr FxTheme::getDefaultTypeface()
{
    return font_400_;
}

FxThemeMode FxTheme::getThemeMode()
{
	return theme_mode_;
}

void FxTheme::setThemeMode(FxThemeMode theme_mode)
{
	auto& theme = dynamic_cast<FxTheme&>(LookAndFeel::getDefaultLookAndFeel());

	theme_mode_ = theme_mode;

	theme.init();
}

uint32 FxTheme::getColor(FxColor color)
{
	return theme_colors_[theme_mode_][color];
}

const char* FxTheme::getImage(FxImage image)
{
	return theme_images_[theme_mode_][image];
}

const int FxTheme::getImageSize(FxImage image)
{
	return theme_image_sizes_[theme_mode_][image];
}

Rectangle<int> FxTheme::getTooltipBounds(const String& tipText, Point<int> screenPos, Rectangle<int> parentArea)
{
    const TextLayout tl(layoutTooltipText(tipText, Colours::black));

    auto w = (int)(tl.getWidth() + 20.0f);
    auto h = (int)(tl.getHeight() + 12.0f);

    return Rectangle<int>(screenPos.x > parentArea.getCentreX() ? screenPos.x - (w + 18) : screenPos.x + 36,
        screenPos.y > parentArea.getCentreY() ? screenPos.y - (h + 12) : screenPos.y + 12,
        w, h)
        .constrainedWithin(parentArea);
}

void FxTheme::drawTooltip(Graphics& g, const String& text, int width, int height)
{
    Rectangle<int> bounds(width, height);
    auto cornerSize = 5.0f;

    g.setColour(findColour(TooltipWindow::backgroundColourId));
    g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

    g.setColour(findColour(TooltipWindow::outlineColourId));
    g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    layoutTooltipText(text, findColour(TooltipWindow::textColourId))
        .draw(g, bounds.toFloat().reduced(10, 0));
}

void FxTheme::drawDocumentWindowTitleBar(DocumentWindow& window, Graphics& g,
										int w, int h, int titleSpaceX, int titleSpaceW,
										const Image* icon, bool drawTitleTextOnLeft)
{
	if (w * h == 0)
		return;

	auto isActive = window.isActiveWindow();

	g.setColour(getCurrentColourScheme().getUIColour(ColourScheme::widgetBackground));
	g.fillAll();

	Font font(12.0f, Font::plain);
	g.setFont(font);

	auto textW = font.getStringWidth(window.getName());
	auto iconW = 0;
	auto iconH = 0;

	if (icon != nullptr)
	{
		iconH = static_cast<int> (font.getHeight());
		iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
	}

	textW = jmin(titleSpaceW, textW + iconW);
	auto textX = drawTitleTextOnLeft ? titleSpaceX
		: jmax(titleSpaceX, (w - textW) / 2);

	if (textX + textW > titleSpaceX + titleSpaceW)
		textX = titleSpaceX + titleSpaceW - textW;

	if (icon != nullptr)
	{
		g.setOpacity(isActive ? 1.0f : 0.6f);
		g.drawImageWithin(*icon, textX, (h - iconH) / 2, iconW, iconH,
			RectanglePlacement::centred, false);
		textX += iconW;
		textW -= iconW;
	}

	if (window.isColourSpecified(DocumentWindow::textColourId) || isColourSpecified(DocumentWindow::textColourId))
		g.setColour(window.findColour(DocumentWindow::textColourId));
	else
		g.setColour(getCurrentColourScheme().getUIColour(ColourScheme::defaultText));

	g.drawText(window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
}

class FxTheme_DocumentWindowButton : public Button
{
public:
	FxTheme_DocumentWindowButton(const String& name, Colour c, const Path& normal, const Path& toggled)
		: Button(name), colour(c), normalShape(normal), toggledShape(toggled)
	{
	}

	void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
	{
		auto background = Colours::grey;

		if (auto* rw = findParentComponentOfClass<ResizableWindow>())
			if (auto lf = dynamic_cast<LookAndFeel_V4*> (&rw->getLookAndFeel()))
				background = lf->getCurrentColourScheme().getUIColour(LookAndFeel_V4::ColourScheme::widgetBackground);

		g.fillAll(background);

		g.setColour((!isEnabled() || shouldDrawButtonAsDown) ? colour.withAlpha(0.6f)
			: colour);

		if (shouldDrawButtonAsHighlighted)
		{
			g.fillAll();
			g.setColour(background);
		}

		auto& p = getToggleState() ? toggledShape : normalShape;

		auto reducedRect = Justification(Justification::centred)
			.appliedToRectangle(Rectangle<int>(12, 12), getLocalBounds())
			.toFloat();

		g.fillPath(p, p.getTransformToScaleToFit(reducedRect, true));
	}

private:
	Colour colour;
	Path normalShape, toggledShape;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxTheme_DocumentWindowButton)
};

Button* FxTheme::createDocumentWindowButton(int buttonType)
{
	Path shape;
	auto crossThickness = 0.15f;

	if (buttonType == DocumentWindow::closeButton)
	{
		shape.addLineSegment({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
		shape.addLineSegment({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);

		return new FxTheme_DocumentWindowButton("close", Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f), shape, shape);
	}

	if (buttonType == DocumentWindow::minimiseButton)
	{
		shape.addLineSegment({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

		return new FxTheme_DocumentWindowButton("minimise", Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f), shape, shape);
	}

	if (buttonType == DocumentWindow::maximiseButton)
	{
		shape.addLineSegment({ 0.5f, 0.0f, 0.5f, 1.0f }, crossThickness);
		shape.addLineSegment({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

		Path fullscreenShape;
		fullscreenShape.startNewSubPath(45.0f, 100.0f);
		fullscreenShape.lineTo(0.0f, 100.0f);
		fullscreenShape.lineTo(0.0f, 0.0f);
		fullscreenShape.lineTo(100.0f, 0.0f);
		fullscreenShape.lineTo(100.0f, 45.0f);
		fullscreenShape.addRectangle(45.0f, 45.0f, 100.0f, 100.0f);
		PathStrokeType(30.0f).createStrokedPath(fullscreenShape, fullscreenShape);

		return new FxTheme_DocumentWindowButton("maximise", Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f), shape, fullscreenShape);
	}

	jassertfalse;
	return nullptr;
}

TextLayout FxTheme::layoutTooltipText(const String& text, Colour colour) noexcept
{
    const float tooltipFontSize = 14.0f;
    const int maxToolTipWidth = 400;

    AttributedString s;
    s.setWordWrap(AttributedString::WordWrap::byWord);
    s.setJustification(Justification::centredLeft);
    s.append(text, getNormalFont().withHeight(tooltipFontSize), colour);

    TextLayout tl;
    tl.createLayout(s, (float)maxToolTipWidth);
    return tl;
}

Typeface::Ptr FxTheme::loadTypeface(String fileName)
{
    MemoryBlock fontBuffer;
    String filePath = File::addTrailingSeparator(File::getCurrentWorkingDirectory().getFullPathName());
    File fontFile = File(filePath+fileName);
    if (fontFile.exists())
    {
        if (fontFile.loadFileAsData(fontBuffer))
        {
            return Typeface::createSystemTypefaceFor(fontBuffer.getData(), fontBuffer.getSize());
        }
    }
        
    return nullptr;
}