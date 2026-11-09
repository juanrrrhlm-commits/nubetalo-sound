#include "SpectrumTap.h"
#include <cstdio>
#include <vector>
int main()
{
	const int sr = 48000, N = 48;
	for (double f : { 100.0, 1000.0, 8000.0 })
	{
		SpectrumTap t; std::vector<float> buf(8192 * 2);
		for (int i = 0; i < 8192; i++) { float s = 0.5f * (float)std::sin(2 * 3.14159265358979 * f * i / sr); buf[i * 2] = s; buf[i * 2 + 1] = s; }
		t.push(buf.data(), 8192, 2, sr);
		float bars[N]; t.compute(bars, N);
		int best = 0; for (int i = 1; i < N; i++) if (bars[i] > bars[best]) best = i;
		const double lo = 30.0, hi = 18000.0 * 1.0;
		double fa = lo * std::pow(std::min(hi, sr * 0.5 * 0.95) / lo, (double)best / N), fb = lo * std::pow(std::min(hi, sr * 0.5 * 0.95) / lo, (double)(best + 1) / N);
		printf("tono %6.0f Hz -> barra %2d (%.0f-%.0f Hz) valor %.3f\n", f, best, fa, fb, bars[best]);
	}
	SpectrumTap t2; float bars[N]; bool ok = t2.compute(bars, N); printf("sin senal: ok=%d\n", (int)ok);
	return 0;
}
