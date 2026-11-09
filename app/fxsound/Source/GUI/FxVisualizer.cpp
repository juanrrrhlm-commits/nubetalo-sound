/*
FxSound
Copyright (C) 2025  FxSound LLC

Contributors:
    www.theremino.com (2025)

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

#include "FxVisualizer.h"
#include "FxController.h"
#include "FxTheme.h"

FxVisualizer::FxVisualizer()
{
    band_values_.resize(FxController::NUM_SPECTRUM_BANDS);
    band_graph_.resize(FxController::NUM_SPECTRUM_BANDS * NUM_BARS);
    raw_.assign(BAR_COUNT, 0.0f); shown_.assign(BAR_COUNT, 0.0f); peak_.assign(BAR_COUNT, 0.0f); peak_v_.assign(BAR_COUNT, 0.0f);

#if JUCE_MAJOR_VERSION >= 8
    start();
#else
    calcGradient();
    reset();
    setFramesPerSecond(10);
#endif

    setOpaque(false);
    setSize(WIDTH, HEIGHT);
}

void FxVisualizer::start()
{    
    calcGradient();

#if JUCE_MAJOR_VERSION >= 8
    if (vblank_listener_ == nullptr)
    {
        vblank_listener_ = std::make_unique<juce::VBlankAttachment>(
            this,
            [this](double timestamp)
            {
                if (!isShowing())
                    return;

                static double last_frame_time = 0.0;
                constexpr double fps_interval = 1.0 / 30.0;

                if (timestamp - last_frame_time >= fps_interval)
                {
                    last_frame_time = timestamp;
                    if (FxController::getInstance().isAudioProcessing())
                    {
                        update();
                        repaint();
                    }
                    else
                    {
                        reset();
                        repaint();
                        vblank_listener_.reset();
                    }
                }
            });
    }
#else
    setFramesPerSecond(30);
#endif
}

void FxVisualizer::pause()
{
    calcGradient();

#if JUCE_MAJOR_VERSION >= 8
    reset();
    repaint();

    if (vblank_listener_ != nullptr)
    {
        vblank_listener_.reset();
    }
#else
    setFramesPerSecond(10);
#endif
}

void FxVisualizer::reset()
{
    std::fill(shown_.begin(), shown_.end(), 0.0f);
    std::fill(peak_.begin(), peak_.end(), 0.0f);
    std::fill(peak_v_.begin(), peak_v_.end(), 0.0f);
}

void FxVisualizer::update()
{
    if (!isEnabled())
        return;

    FxController::getInstance().getSpectrumBars(raw_);

    for (int i = 0; i < BAR_COUNT; i++)
    {
        // fast attack, slow release; the peak marker falls with gravity
        const float v = raw_[i];
        float s = shown_[i];
        s += (v - s) * (v > s ? 0.55f : 0.14f);
        shown_[i] = s;

        if (s >= peak_[i])
        {
            peak_[i] = s;
            peak_v_[i] = 0.0f;
        }
        else
        {
            peak_v_[i] += 0.0022f;
            peak_[i] = jmax(s, peak_[i] - peak_v_[i]);
        }
    }
}

void FxVisualizer::paint(Graphics& g)
{
    auto& theme = dynamic_cast<FxTheme&>(getLookAndFeel());
    auto bounds = getLocalBounds().toFloat();

    g.setColour(FXCOL(ControlBackground));
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(FXCOL(Outline));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);

    const float left = 40.0f, right = 16.0f, top = 16.0f, bottom = 26.0f;
    const auto plot = bounds.withTrimmedLeft(left).withTrimmedRight(right).withTrimmedTop(top).withTrimmedBottom(bottom);
    const Colour accent = isEnabled() ? FXCOL(SliderTrack) : FXCOL(HintText);
    const Colour light = isEnabled() ? Colour(0xff8dbfff) : FXCOL(HintText);

    // ---- grid and axis labels ----
    g.setFont(theme.getNormalFont().withHeight(11.0f));
    for (int db : { 0, -20, -40, -60 })
    {
        const float y = plot.getBottom() - plot.getHeight() * (float)(db + 80) / 80.0f;
        g.setColour(FXCOL(Outline).withAlpha(db == 0 ? 0.0f : 0.8f));
        g.drawLine(plot.getX(), y, plot.getRight(), y, 1.0f);
        g.setColour(FXCOL(HintText));
        g.drawText(String(db), juce::Rectangle<float>(bounds.getX() + 4.0f, y - 7.0f, left - 10.0f, 14.0f), Justification::centredRight);
    }
    const double f_lo = 30.0, f_hi = 18000.0;
    auto xForFreq = [&](double f) { return plot.getX() + plot.getWidth() * (float)(std::log(f / f_lo) / std::log(f_hi / f_lo)); };
    struct Tick { double f; const char* label; };
    for (auto t : { Tick{ 50, "50" }, Tick{ 100, "100" }, Tick{ 200, "200" }, Tick{ 500, "500" }, Tick{ 1000, "1k" }, Tick{ 2000, "2k" }, Tick{ 5000, "5k" }, Tick{ 10000, "10k" } })
    {
        const float x = xForFreq(t.f);
        g.setColour(FXCOL(Outline).withAlpha(0.5f));
        g.drawLine(x, plot.getY(), x, plot.getBottom(), 1.0f);
        g.setColour(FXCOL(HintText));
        g.drawText(t.label, juce::Rectangle<float>(x - 20.0f, plot.getBottom() + 6.0f, 40.0f, 14.0f), Justification::centred);
    }

    // ---- data (FXSOUND_DEMO=1 draws a fixed pattern for screenshots) ----
#ifndef NUBETALO_RELEASE
    static const bool demo = SystemStats::getEnvironmentVariable("FXSOUND_DEMO", "") == "1";
#else
    static const bool demo = false;
#endif
    auto level = [&](int i) -> float {
        if (!demo) return shown_[i];
        const float t = (float)i / BAR_COUNT;
        return jlimit(0.0f, 1.0f, 0.18f + 0.55f * std::exp(-t * 2.4f) + 0.12f * std::abs(std::sin(i * 0.7f)) - 0.1f * t);
    };
    auto peak = [&](int i) -> float { return demo ? jmin(1.0f, level(i) + 0.07f) : peak_[i]; };

    float strongest = 0.0f;
    for (int i = 0; i < BAR_COUNT; i++) strongest = jmax(strongest, level(i));

    if (strongest < 0.02f)
    {
        g.setColour(accent.withAlpha(0.5f));
        g.fillRect(plot.getX(), plot.getBottom() - 1.5f, plot.getWidth(), 1.5f);
        return;
    }

    // ---- bars ----
    const float gap = 2.5f;
    const float bar_w = (plot.getWidth() - gap * (BAR_COUNT - 1)) / BAR_COUNT;
    for (int i = 0; i < BAR_COUNT; i++)
    {
        const float h = jmax(2.0f, level(i) * plot.getHeight());
        const float x = plot.getX() + i * (bar_w + gap);
        const auto r = juce::Rectangle<float>(x, plot.getBottom() - h, bar_w, h);

        g.setColour(accent.withAlpha(0.10f));
        g.fillRoundedRectangle(r.expanded(2.0f, 3.0f), bar_w * 0.5f + 2.0f);

        g.setGradientFill(ColourGradient(light.withAlpha(0.95f), 0.0f, r.getY(), accent.withAlpha(0.35f), 0.0f, plot.getBottom(), false));
        Path p;
        p.addRoundedRectangle(r.getX(), r.getY(), r.getWidth(), r.getHeight(), bar_w * 0.5f, bar_w * 0.5f, true, true, false, false);
        g.fillPath(p);

        const float py = plot.getBottom() - jmax(2.0f, peak(i) * plot.getHeight());
        if (py < r.getY() - 2.5f)
        {
            g.setColour(light.withAlpha(0.9f));
            g.fillRoundedRectangle(x, py - 2.0f, bar_w, 2.0f, 1.0f);
        }
    }
}

void FxVisualizer::enablementChanged()
{
    if (isEnabled())
    {
        start();
    }
    else
    {
        pause();
        reset();
    }
}

void FxVisualizer::lookAndFeelChanged()
{
    calcGradient();
    repaint();
}

void FxVisualizer::calcGradient()
{
    float alpha = 0.75;
    if (FxController::getInstance().isAudioProcessing())
    {
        alpha = 1.0;
    }

    gradient_ = ColourGradient(isEnabled() ? Colour(FXCOLOR(GraphHigh)).withAlpha(alpha) : Colour(FXCOLOR(GraphHigh)).withSaturation(0.0f).withAlpha(alpha),
        2.0f, 0.0f,
        isEnabled() ? Colour(FXCOLOR(GraphHigh)).withAlpha(alpha) : Colour(FXCOLOR(GraphHigh)).withSaturation(0.0f).withAlpha(alpha),
        2.0f, 100.0f, false);
    if (isEnabled())
    {
        gradient_.addColour(0.5f, Colour(FXCOLOR(GraphLow)).withAlpha(alpha));
    }
    else
    {
        gradient_.addColour(0.5f, Colour(FXCOLOR(GraphLow)).withSaturation(0.0f).withAlpha(alpha));
    }
}