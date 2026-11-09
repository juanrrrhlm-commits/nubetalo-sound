// Real-time spectrum for the visualizer.
// The audio thread pushes the processed samples; the UI thread asks for log-spaced bars.
// Bars are in 0..1 (0 = -80 dB, 1 = 0 dBFS) with a gentle 4.5 dB/octave tilt so music looks balanced.
#pragma once

#include <atomic>
#include <cmath>
#include <vector>

class SpectrumTap
{
public:
	static constexpr int FFT_SIZE = 4096;
	static constexpr int RING_SIZE = 16384; // power of two, > FFT_SIZE

	SpectrumTap() : ring_(RING_SIZE, 0.0f), window_(FFT_SIZE), re_(FFT_SIZE), im_(FFT_SIZE)
	{
		const double pi = 3.14159265358979323846;
		window_sum_ = 0.0;
		for (int i = 0; i < FFT_SIZE; i++)
		{
			window_[i] = (float)(0.5 - 0.5 * std::cos(2.0 * pi * i / (FFT_SIZE - 1)));
			window_sum_ += window_[i];
		}
	}

	// Audio thread. `data` is interleaved float.
	void push(const float* data, int frames, int channels, int sample_rate)
	{
		if (frames <= 0 || channels <= 0 || data == nullptr)
			return;
		sample_rate_.store(sample_rate, std::memory_order_relaxed);
		unsigned w = write_.load(std::memory_order_relaxed);
		const float inv = 1.0f / (float)channels;
		for (int i = 0; i < frames; i++)
		{
			float m = 0.0f;
			for (int c = 0; c < channels; c++)
				m += data[i * channels + c];
			ring_[w & (RING_SIZE - 1)] = m * inv;
			w++;
		}
		write_.store(w, std::memory_order_release);
	}

	// UI thread. Fills `bars[0..n-1]` with values in 0..1. Returns false when there is no signal yet.
	bool compute(float* bars, int n)
	{
		const int sr = sample_rate_.load(std::memory_order_relaxed);
		if (sr <= 0 || n <= 0)
		{
			for (int i = 0; i < n; i++) bars[i] = 0.0f;
			return false;
		}

		const unsigned w = write_.load(std::memory_order_acquire);
		for (int i = 0; i < FFT_SIZE; i++)
		{
			re_[i] = ring_[(w - FFT_SIZE + i) & (RING_SIZE - 1)] * window_[i];
			im_[i] = 0.0f;
		}
		fft();

		const double bin_hz = (double)sr / FFT_SIZE;
		const double f_lo = 30.0, f_hi = (std::min)(18000.0, sr * 0.5 * 0.95);
		const int half = FFT_SIZE / 2;
		mag_.resize(half);
		for (int k = 0; k < half; k++)
			mag_[k] = (float)(2.0 * std::sqrt((double)re_[k] * re_[k] + (double)im_[k] * im_[k]) / window_sum_);

		for (int b = 0; b < n; b++)
		{
			const double fa = f_lo * std::pow(f_hi / f_lo, (double)b / n);
			const double fb = f_lo * std::pow(f_hi / f_lo, (double)(b + 1) / n);
			double a = fa / bin_hz, z = fb / bin_hz;
			double amp;
			if (z - a < 1.0)
			{
				// narrow bar (low frequencies): interpolate between the two nearest bins
				const double c = 0.5 * (a + z);
				const int k = (std::min)(half - 2, (std::max)(0, (int)c));
				const double t = c - k;
				amp = mag_[k] * (1.0 - t) + mag_[k + 1] * t;
			}
			else
			{
				double peak = 0.0, sum = 0.0; int cnt = 0;
				for (int k = (int)std::ceil(a); k <= (int)z && k < half; k++) { peak = (std::max)(peak, (double)mag_[k]); sum += (double)mag_[k] * mag_[k]; cnt++; }
				amp = cnt ? (0.6 * peak + 0.4 * std::sqrt(sum / cnt)) : 0.0;
			}
			const double fc = std::sqrt(fa * fb);
			double db = 20.0 * std::log10(amp + 1e-9) + 4.5 * std::log2(fc / 1000.0);
			double v = (db + 80.0) / 80.0;
			bars[b] = (float)(std::min)(1.0, (std::max)(0.0, v));
		}
		return true;
	}

private:
	void fft()
	{
		const int n = FFT_SIZE;
		for (int i = 1, j = 0; i < n; i++)
		{
			int bit = n >> 1;
			for (; j & bit; bit >>= 1) j ^= bit;
			j ^= bit;
			if (i < j) { std::swap(re_[i], re_[j]); std::swap(im_[i], im_[j]); }
		}
		const double pi = 3.14159265358979323846;
		for (int len = 2; len <= n; len <<= 1)
		{
			const double ang = -2.0 * pi / len;
			const float wr = (float)std::cos(ang), wi = (float)std::sin(ang);
			for (int i = 0; i < n; i += len)
			{
				float cr = 1.0f, ci = 0.0f;
				for (int k = 0; k < len / 2; k++)
				{
					const int a = i + k, b = i + k + len / 2;
					const float tr = re_[b] * cr - im_[b] * ci, ti = re_[b] * ci + im_[b] * cr;
					re_[b] = re_[a] - tr; im_[b] = im_[a] - ti;
					re_[a] += tr; im_[a] += ti;
					const float ncr = cr * wr - ci * wi;
					ci = cr * wi + ci * wr; cr = ncr;
				}
			}
		}
	}

	std::vector<float> ring_, window_, re_, im_, mag_;
	double window_sum_ = 1.0;
	std::atomic<unsigned> write_{ 0 };
	std::atomic<int> sample_rate_{ 0 };
};
