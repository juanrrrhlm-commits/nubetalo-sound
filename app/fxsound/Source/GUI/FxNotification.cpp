/*
FxSound
Copyright (C) 2025  FxSound LLC
Modifications Copyright (C) 2026  Nubetalo

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

#include "FxNotification.h"

namespace
{
	constexpr int TICK_MS = 15;
	constexpr float OPEN_STEP = 0.075f;   // ~200 ms
	constexpr float CLOSE_STEP = 0.095f;

	float easeOut(float t) { return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }

	// Marca de Nubetalo en miniatura: anillo abierto por arriba y onda.
	void drawMark(Graphics& g, juce::Rectangle<float> box, Colour ring, Colour wave)
	{
		const float s = box.getWidth() / 24.0f;
		const float cx = box.getCentreX(), cy = box.getCentreY(), r = 8.2f * s;
		Path arc;
		arc.addCentredArc(cx, cy, r, r, 0.0f, 0.56f, MathConstants<float>::twoPi - 0.56f, true);
		g.setColour(ring);
		g.strokePath(arc, PathStrokeType(2.2f * s, PathStrokeType::curved, PathStrokeType::rounded));

		Path w;
		const float x0 = box.getX() + 7.4f * s, y0 = box.getY() + 12.6f * s;
		w.startNewSubPath(x0, y0);
		w.cubicTo(box.getX() + 8.9f * s, box.getY() + 8.6f * s, box.getX() + 10.5f * s, box.getY() + 8.6f * s, box.getX() + 12.0f * s, y0);
		w.cubicTo(box.getX() + 13.5f * s, box.getY() + 16.6f * s, box.getX() + 15.1f * s, box.getY() + 16.6f * s, box.getX() + 16.6f * s, y0);
		g.setColour(wave);
		g.strokePath(w, PathStrokeType(1.7f * s, PathStrokeType::curved, PathStrokeType::rounded));
	}
}

FxNotification::FxNotification()
{
	setSize(WIDTH, HEIGHT);
	setOpaque(false);
	setVisible(false);
	setAlwaysOnTop(true);
	setMouseCursor(MouseCursor::PointingHandCursor);
}

FxNotification::~FxNotification()
{
	stopTimer();
	removeFromDesktop();
}

void FxNotification::setMessage(const String& message, const std::pair<String, String>& link, bool autohide)
{
	auto& theme = dynamic_cast<FxTheme&>(LookAndFeel::getDefaultLookAndFeel());
	setLookAndFeel(&LookAndFeel::getDefaultLookAndFeel());

	autohide_ = autohide;
	link_ = link;

	StringArray lines;
	lines.addLines(message);
	for (auto& l : lines) l = l.trim();
	lines.removeEmptyStrings();

	if (autohide)
	{
		text_ = lines.joinIntoString(String(L"  ·  "));
		const auto font = theme.getNormalFont().withHeight(13.5f);
		const int text_w = (int)font.getStringWidth(text_);
		const int link_w = link.first.isEmpty() ? 0 : (int)font.getStringWidth(link.first) + 10;
		const int pill_w = jlimit(150, 460, 14 + 16 + 9 + text_w + link_w + 18);
		setSize(pill_w + MARGIN * 2, PILL_HEIGHT + MARGIN * 2);
	}
	else
	{
		text_ = lines.joinIntoString("\n");
		setSize(MAX_WIDTH, jmax(HEIGHT, lines.size() * 20 + 36));
	}
}

void FxNotification::showMessage(bool autohide)
{
	autohide_ = autohide;
	hold_ticks_ = 0;

	if (autohide)
	{
		progress_ = 0.0f;
		phase_ = Phase::Opening;
		addToDesktop(ComponentPeer::windowIsTemporary);
		setVisible(true);
		toFront(false);
		startTimer(TICK_MS);
	}
	else
	{
		stopTimer();
		phase_ = Phase::Holding;
		progress_ = 1.0f;
		setVisible(true);
		repaint();
	}
}

void FxNotification::close()
{
	if (phase_ == Phase::Hidden)
		return;
	phase_ = Phase::Closing;
	startTimer(TICK_MS);
}

void FxNotification::paint(Graphics& g)
{
	auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
	const auto full = getLocalBounds().toFloat().reduced((float)MARGIN);
	const float t = easeOut(jlimit(0.0f, 1.0f, progress_));

	const float h = full.getHeight();
	const float w = autohide_ ? jmax(h, full.getWidth() * t) : full.getWidth();
	// la píldora crece desde el centro, como la isla dinámica
	const auto r = juce::Rectangle<float>(autohide_ ? full.getCentreX() - w * 0.5f : full.getX(), full.getY(), w, h);
	const float radius = autohide_ ? h * 0.5f : 14.0f;
	const float alpha = autohide_ ? jlimit(0.0f, 1.0f, progress_ * 2.2f) : 1.0f;

	// resplandor tenue del color de la app
	Path shape;
	shape.addRoundedRectangle(r.getX(), r.getY(), r.getWidth(), r.getHeight(), radius);
	DropShadow(FXCOL(SliderTrack).withAlpha(0.30f * alpha), 9, { 0, 0 }).drawForPath(g, shape);

	g.setColour(Colour(0xff07090d).withAlpha(0.97f * alpha));
	g.fillPath(shape);
	g.setColour(FXCOL(SliderTrack).withAlpha(0.38f * alpha));
	g.strokePath(shape, PathStrokeType(1.0f));

	const float content_alpha = autohide_ ? jlimit(0.0f, 1.0f, (t - 0.55f) / 0.45f) : 1.0f;
	const float icon_cx = (autohide_ && w < h + 1.0f) ? r.getX() + w * 0.5f : r.getX() + 24.0f;
	const float icon_cy = autohide_ ? r.getCentreY() : r.getY() + 24.0f;
	const auto icon_box = juce::Rectangle<float>(18.0f, 18.0f).withCentre({ icon_cx, icon_cy });
	drawMark(g, icon_box, FXCOL(SliderTrack).withAlpha(alpha), FXCOL(SliderTrack).brighter(0.6f).withAlpha(alpha));

	if (content_alpha <= 0.0f)
		return;

	const auto font = theme.getNormalFont().withHeight(13.5f);
	g.setFont(font);
	auto text_area = juce::Rectangle<float>(icon_box.getRight() + 9.0f, r.getY(), r.getRight() - icon_box.getRight() - 9.0f - 16.0f, r.getHeight());

	if (autohide_)
	{
		const float link_w = link_.first.isEmpty() ? 0.0f : (float)font.getStringWidth(link_.first) + 10.0f;
		g.setColour(FXCOL(HighlightedText).withAlpha(content_alpha));
		g.drawText(text_, text_area.withTrimmedRight(link_w), Justification::centredLeft, true);
		if (link_w > 0.0f)
		{
			g.setColour(FXCOL(SliderTrack).brighter(0.5f).withAlpha(content_alpha));
			g.drawText(link_.first, text_area.removeFromRight(link_w), Justification::centredRight, true);
		}
	}
	else
	{
		// tarjeta: texto en varias líneas y el enlace al final
		g.setColour(FXCOL(HighlightedText));
		const auto body = text_area.withTrimmedTop(6.0f);
		g.drawFittedText(text_, body.toNearestInt(), Justification::topLeft, 5, 1.0f);
		if (link_.first.isNotEmpty())
		{
			const auto lines = StringArray::fromLines(text_);
			const float y = r.getY() + 12.0f + lines.size() * 18.0f;
			g.setColour(FXCOL(SliderTrack).brighter(0.5f));
			g.drawText(link_.first, juce::Rectangle<float>(body.getX(), y, body.getWidth(), 20.0f), Justification::centredLeft, true);
		}
	}
}

void FxNotification::mouseUp(const MouseEvent&)
{
	if (link_.second.isNotEmpty())
		URL(link_.second).launchInDefaultBrowser();
	if (autohide_)
		close();
}

void FxNotification::timerCallback()
{
	switch (phase_)
	{
	case Phase::Opening:
		progress_ += OPEN_STEP;
		if (progress_ >= 1.0f)
		{
			progress_ = 1.0f;
			phase_ = Phase::Holding;
			hold_ticks_ = 0;
		}
		break;

	case Phase::Holding:
	{
		const int hold_ms = link_.second.isNotEmpty() ? 6500 : 3200;
		if (++hold_ticks_ * TICK_MS >= hold_ms)
			phase_ = Phase::Closing;
		break;
	}

	case Phase::Closing:
		progress_ -= CLOSE_STEP;
		if (progress_ <= 0.0f)
		{
			progress_ = 0.0f;
			phase_ = Phase::Hidden;
			stopTimer();
			setVisible(false);
			removeFromDesktop();
			return;
		}
		break;

	case Phase::Hidden:
		stopTimer();
		return;
	}
	repaint();
}
