// Fila de ajustes de Nubetalo Sound: tarjeta con título, detalle y un control a la derecha
// (interruptor, valor con menú, enlace o combinación de teclas que se graba al pulsarla).
#pragma once

#include <JuceHeader.h>
#include "FxTheme.h"

class FxSettingRow : public Component
{
public:
	enum class Kind { Switch, Value, Link, Key };

	FxSettingRow(Kind kind, const String& title, const String& subtitle = String())
		: kind_(kind), title_(title), subtitle_(subtitle)
	{
		setWantsKeyboardFocus(true);
		setMouseCursor(MouseCursor::PointingHandCursor);
	}

	std::function<void()> onClick;                    // interruptor cambiado / valor o enlace pulsado / empieza a grabar
	std::function<bool(const KeyPress&)> onKeyPress;  // Key: true si la combinación se aceptó

	void setTitle(const String& t) { title_ = t; repaint(); }
	void setSubtitle(const String& t) { subtitle_ = t; repaint(); }
	void setValue(const String& v) { value_ = v; repaint(); }
	void setValueFont(const Font& f) { value_font_ = f; has_value_font_ = true; repaint(); }
	void setOn(bool on) { on_ = on; repaint(); }
	bool isOn() const { return on_; }

	void paint(Graphics& g) override
	{
		auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
		auto r = getLocalBounds().toFloat();
		const float alpha = isEnabled() ? 1.0f : 0.45f;

		g.setColour((hover_ && isEnabled() ? FXCOL(ControlBackground).brighter(0.08f) : FXCOL(ControlBackground)).withMultipliedAlpha(alpha));
		g.fillRoundedRectangle(r, 10.0f);
		g.setColour((recording_ ? FXCOL(SliderTrack) : (hasKeyboardFocus(true) ? FXCOL(SliderTrack).withAlpha(0.6f) : FXCOL(Outline))).withMultipliedAlpha(alpha));
		g.drawRoundedRectangle(r.reduced(0.5f), 10.0f, 1.0f);

		// control de la derecha
		auto right = r.withTrimmedRight(14.0f);
		float text_right = right.getRight();

		if (kind_ == Kind::Switch)
		{
			const auto sw = juce::Rectangle<float>(40.0f, 22.0f).withCentre({ right.getRight() - 20.0f, r.getCentreY() });
			g.setColour((on_ ? FXCOL(SliderTrack) : FXCOL(Outline).brighter(0.15f)).withMultipliedAlpha(alpha));
			g.fillRoundedRectangle(sw, 11.0f);
			g.setColour(Colours::white.withMultipliedAlpha(alpha * (on_ ? 1.0f : 0.85f)));
			const float knob = 16.0f;
			g.fillEllipse(juce::Rectangle<float>(knob, knob).withCentre({ on_ ? sw.getRight() - 11.0f : sw.getX() + 11.0f, sw.getCentreY() }));
			text_right = sw.getX() - 12.0f;
		}
		else if (kind_ == Kind::Key)
		{
			auto font = theme.getNormalFont().withHeight(13.0f);
			const String txt = recording_ ? TRANS("Press a combination...") : (value_.isEmpty() ? TRANS("None") : value_);
			const float w = font.getStringWidth(txt) + 24.0f;
			const auto chip = juce::Rectangle<float>(w, 26.0f).withCentre({ right.getRight() - w * 0.5f, r.getCentreY() });
			g.setColour(FXCOL(WidgetBackground).withMultipliedAlpha(alpha));
			g.fillRoundedRectangle(chip, 7.0f);
			g.setColour((recording_ ? FXCOL(SliderTrack) : FXCOL(Outline).brighter(0.25f)).withMultipliedAlpha(alpha));
			g.drawRoundedRectangle(chip.reduced(0.5f), 7.0f, 1.0f);
			g.setColour((recording_ ? FXCOL(SliderTrack).brighter(0.4f) : FXCOL(HighlightedText)).withMultipliedAlpha(alpha));
			g.setFont(font);
			g.drawText(txt, chip, Justification::centred);
			text_right = chip.getX() - 10.0f;
		}
		else if (kind_ == Kind::Value)
		{
			// chevron
			const float cx = right.getRight() - 6.0f, cy = r.getCentreY();
			Path p; p.startNewSubPath(cx - 3.0f, cy - 5.0f); p.lineTo(cx + 2.0f, cy); p.lineTo(cx - 3.0f, cy + 5.0f);
			g.setColour(FXCOL(HintText).withMultipliedAlpha(alpha));
			g.strokePath(p, PathStrokeType(1.6f, PathStrokeType::curved, PathStrokeType::rounded));
			auto font = has_value_font_ ? value_font_ : theme.getNormalFont().withHeight(14.0f);
			const float w = (float)font.getStringWidth(value_) + 4.0f;
			g.setColour(FXCOL(HintText).brighter(0.15f).withMultipliedAlpha(alpha));
			g.setFont(font);
			g.drawText(value_, juce::Rectangle<float>(cx - 16.0f - w, 0.0f, w, r.getHeight()), Justification::centredRight);
			text_right = cx - 16.0f - w - 8.0f;
		}
		else // Link: flecha diagonal
		{
			const float cx = right.getRight() - 8.0f, cy = r.getCentreY();
			Path p; p.startNewSubPath(cx - 4.0f, cy + 4.0f); p.lineTo(cx + 4.0f, cy - 4.0f);
			p.startNewSubPath(cx - 1.5f, cy - 4.0f); p.lineTo(cx + 4.0f, cy - 4.0f); p.lineTo(cx + 4.0f, cy + 1.5f);
			g.setColour(FXCOL(SliderTrack).brighter(0.2f).withMultipliedAlpha(alpha));
			g.strokePath(p, PathStrokeType(1.6f, PathStrokeType::curved, PathStrokeType::rounded));
			text_right = cx - 14.0f;
		}

		// textos
		auto text_area = r.withTrimmedLeft(16.0f).withRight(text_right - 4.0f);
		g.setColour(FXCOL(HighlightedText).withMultipliedAlpha(alpha));
		g.setFont(theme.getNormalFont().withHeight(14.5f));
		if (subtitle_.isEmpty())
		{
			g.drawText(TRANS(title_), text_area, Justification::centredLeft, true);
		}
		else
		{
			g.drawText(TRANS(title_), text_area.withTrimmedTop(7.0f).withHeight(20.0f), Justification::centredLeft, true);
			g.setColour(FXCOL(HintText).withMultipliedAlpha(alpha));
			g.setFont(theme.getNormalFont().withHeight(12.0f));
			g.drawText(TRANS(subtitle_), text_area.withTrimmedTop(26.0f).withHeight(16.0f), Justification::centredLeft, true);
		}
	}

	void mouseEnter(const MouseEvent&) override { hover_ = true; repaint(); }
	void mouseExit(const MouseEvent&) override { hover_ = false; repaint(); }

	void mouseUp(const MouseEvent& e) override
	{
		if (!isEnabled() || !contains(e.getPosition()))
			return;
		activate();
	}

	bool keyPressed(const KeyPress& key) override
	{
		if (recording_)
		{
			if (key == KeyPress::escapeKey) { recording_ = false; repaint(); return true; }
			if (onKeyPress && onKeyPress(key)) { recording_ = false; repaint(); }
			return true; // mientras se graba, nada más recibe las teclas
		}
		if (isEnabled() && (key == KeyPress::returnKey || key == KeyPress::spaceKey))
		{
			activate();
			return true;
		}
		return false;
	}

	void focusLost(FocusChangeType) override { recording_ = false; repaint(); }
	void focusGained(FocusChangeType) override { repaint(); }

private:
	void activate()
	{
		if (kind_ == Kind::Switch) on_ = !on_;
		if (kind_ == Kind::Key) { recording_ = true; grabKeyboardFocus(); }
		repaint();
		if (onClick) onClick();
	}

	Kind kind_;
	String title_, subtitle_, value_;
	Font value_font_;
	bool has_value_font_ = false, on_ = false, hover_ = false, recording_ = false;
};
