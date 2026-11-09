// Pruebas del motor DSP con tonos: ganancia, ecualizador, efectos, balance.
#include "DfxDsp.h"
#include <cmath>
#include <cstdio>
#include <vector>


#include <random>
static double run_noise(DfxDsp& dsp, double rms_dbfs, int seconds = 6)
{
	const int frames = 480, sr = 48000; std::mt19937 rng(1); std::normal_distribution<float> nd(0.f, 1.f);
	float amp = (float)std::pow(10.0, rms_dbfs / 20.0); std::vector<float> buf(frames * 2);
	double sum = 0; long n = 0; int blocks = seconds * sr / frames; float lp = 0;
	for (int b = 0; b < blocks; b++)
	{
		for (int i = 0; i < frames; i++) { lp += 0.15f * (nd(rng) - lp); float s = lp * amp * 3.2f; buf[i*2] = s; buf[i*2+1] = s; }
		dsp.processAudio((short*)buf.data(), (short*)buf.data(), frames, 0);
		if (b >= blocks / 2) for (float v : buf) { sum += (double)v * v; n++; }
	}
	return 10 * std::log10(sum / n + 1e-20);
}

struct Meas { double l, r; };

static Meas run(DfxDsp& dsp, double freq, float amp, int seconds = 3)
{
	const int frames = 480, ch = 2, sr = 48000;
	std::vector<float> buf(frames * ch);
	double phase = 0, sl = 0, sr_ = 0; long n = 0;
	int blocks = seconds * sr / frames;
	for (int b = 0; b < blocks; b++)
	{
		for (int i = 0; i < frames; i++)
		{
			float s = amp * (float)std::sin(phase); phase += 2 * 3.14159265358979 * freq / sr;
			buf[i * 2] = s; buf[i * 2 + 1] = s;
		}
		dsp.processAudio((short*)buf.data(), (short*)buf.data(), frames, 0);
		if (b >= blocks / 2)
			for (int i = 0; i < frames; i++) { sl += (double)buf[i*2]*buf[i*2]; sr_ += (double)buf[i*2+1]*buf[i*2+1]; n++; }
	}
	return { 10 * std::log10(sl / n + 1e-20), 10 * std::log10(sr_ / n + 1e-20) };
}

static void reset(DfxDsp& d)
{
	d.setMasterGain(0); d.setBalance(0); d.setVolumeLeveling(0); d.setNormalization(0);
	for (int e = 0; e < DfxDsp::NumEffects; e++) d.setEffectValue((DfxDsp::Effect)e, 0.0f);
	for (int b = 0; b < d.getNumEqBands(); b++) d.setEqBandBoostCut(b, 0.0f);
}

int main()
{
	DfxDsp dsp;
	dsp.setSignalFormat(32, 2, 48000, 32);
	dsp.powerOn(true);
	dsp.setNumBands(10);
	const float A = 0.02f; // -34 dBFS: lejos del limitador

	reset(dsp);
	double base = run(dsp, 1000, A).l;
	printf("Referencia 1 kHz plano: %.1f dB\n", base);

	printf("\n== Ganancia maestra (tono 1 kHz, nivel bajo) ==\n");
	for (float g : { -20.f, -10.f, 0.f, 10.f, 20.f, 30.f }) { reset(dsp); dsp.setMasterGain(g); printf("  %+5.0f dB -> %+6.1f dB respecto a plano\n", g, run(dsp, 1000, A).l - base); }

	printf("\n== Ecualizador 10 bandas: +12 dB y -12 dB en cada banda, tono en su frecuencia central ==\n");
	int nb = dsp.getNumEqBands();
	for (int b = 0; b < nb; b++)
	{
		float f = dsp.getEqBandFrequency(b);
		reset(dsp); double flat = run(dsp, f, A).l;
		reset(dsp); dsp.setEqBandBoostCut(b, 12.0f); double up = run(dsp, f, A).l;
		reset(dsp); dsp.setEqBandBoostCut(b, -12.0f); double dn = run(dsp, f, A).l;
		printf("  banda %2d (%7.0f Hz): +12 -> %+5.1f dB | -12 -> %+5.1f dB\n", b, f, up - flat, dn - flat);
	}

	printf("\n== Efectos 0 -> 10 ==\n");
	struct E { DfxDsp::Effect e; const char* n; double f; } es[] = { {DfxDsp::Bass,"Graves (80 Hz)",80}, {DfxDsp::Fidelity,"Claridad (8 kHz)",8000}, {DfxDsp::Ambience,"Ambiente (1 kHz)",1000}, {DfxDsp::Surround,"Envolvente (1 kHz)",1000}, {DfxDsp::DynamicBoost,"Refuerzo dinamico (1 kHz)",1000} };
	for (auto& x : es)
	{
		reset(dsp); double off = run(dsp, x.f, A).l;
		reset(dsp); dsp.setEffectValue(x.e, 1.0f); Meas on = run(dsp, x.f, A);
		printf("  %-26s %+5.1f dB (L-R con efecto: %+.1f dB)\n", x.n, on.l - off, on.l - on.r);
	}

	printf("\n== Balance (1 kHz): izquierda vs derecha ==\n");
	for (float bal : { -20.f, -10.f, 0.f, 10.f, 20.f }) { reset(dsp); dsp.setBalance(bal); Meas m = run(dsp, 1000, A); printf("  balance %+4.0f -> L %+6.1f  R %+6.1f dB\n", bal, m.l - base, m.r - base); }

	printf("\n== Nivelacion de volumen (tono bajo -50 dBFS, 12 s) ==\n");
	reset(dsp); double q0 = run(dsp, 1000, 0.003f, 12).l; reset(dsp); dsp.setVolumeLeveling(4.0f); double q1 = run(dsp, 1000, 0.003f, 12).l; printf("  0 -> 4: %+5.1f dB\n", q1 - q0);

	printf("\n== Tono fuerte (amplitud 0.85, pico -1.4 dBFS): ganancia maestra ==\n");
	reset(dsp); double loud0 = run(dsp, 1000, 0.85f).l;
	for (float g : { -10.f, 0.f, 6.f, 12.f, 20.f }) { reset(dsp); dsp.setMasterGain(g); printf("  %+4.0f dB -> %+6.1f dB respecto a 0 dB\n", g, run(dsp, 1000, 0.85f).l - loud0); }
	printf("\n== Tono fuerte con Refuerzo dinamico 10 (preset Volume Boost) ==\n");
	reset(dsp); dsp.setEffectValue(DfxDsp::DynamicBoost, 1.0f); double db0 = run(dsp, 1000, 0.85f).l;
	for (float g : { -10.f, 0.f, 6.f, 12.f, 20.f }) { reset(dsp); dsp.setEffectValue(DfxDsp::DynamicBoost, 1.0f); dsp.setMasterGain(g); printf("  %+4.0f dB -> %+6.1f dB respecto a 0 dB\n", g, run(dsp, 1000, 0.85f).l - db0); }

printf("\n== Senal tipo musica (-20 dBFS RMS): ganancia maestra ==\n");
reset(dsp); double m0 = run_noise(dsp, -20.0);
for (float g : { -10.f, 0.f, 6.f, 12.f, 20.f }) { reset(dsp); dsp.setMasterGain(g); printf("  %+4.0f dB -> %+6.1f dB\n", g, run_noise(dsp, -20.0) - m0); }
printf("\n== Senal tipo musica fuerte (-10 dBFS RMS) ==\n");
reset(dsp); double m1 = run_noise(dsp, -10.0);
for (float g : { -10.f, 0.f, 6.f, 12.f, 20.f }) { reset(dsp); dsp.setMasterGain(g); printf("  %+4.0f dB -> %+6.1f dB\n", g, run_noise(dsp, -10.0) - m1); }
return 0;
}
