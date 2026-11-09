// Mide lo que realmente sale por un dispositivo de reproducción (captura "loopback" WASAPI).
// Uso: loopback.exe <texto del nombre del dispositivo> <segundos> [f1 f2 f3 ...]
// Imprime RMS izquierdo/derecho en dBFS y la energía (dB) en cada frecuencia pedida.
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

static std::wstring lower(std::wstring s) { for (auto& c : s) c = (wchar_t)towlower(c); return s; }

int wmain(int argc, wchar_t** argv)
{
	if (argc < 2) { wprintf(L"uso: loopback <nombre> <segundos> [freqs]\n"); return 2; }
	if (std::wstring(argv[1]) == L"--default")
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		IMMDeviceEnumerator* e2 = nullptr; CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&e2));
		IMMDevice* d2 = nullptr; e2->GetDefaultAudioEndpoint(eRender, eConsole, &d2);
		IPropertyStore* p2 = nullptr; d2->OpenPropertyStore(STGM_READ, &p2);
		PROPVARIANT v2; PropVariantInit(&v2); p2->GetValue(PKEY_Device_FriendlyName, &v2);
		wprintf(L"default=%s\n", v2.pwszVal ? v2.pwszVal : L"?");
		return 0;
	}
	if (argc < 3) { wprintf(L"uso: loopback <nombre> <segundos> [freqs]\n"); return 2; }
	std::wstring want = lower(argv[1]);
	double seconds = _wtof(argv[2]);
	std::vector<double> freqs;
	for (int i = 3; i < argc; i++) freqs.push_back(_wtof(argv[i]));

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	IMMDeviceEnumerator* en = nullptr;
	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&en));
	IMMDeviceCollection* col = nullptr;
	en->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &col);
	UINT n = 0; col->GetCount(&n);
	IMMDevice* dev = nullptr; std::wstring chosen;
	for (UINT i = 0; i < n; i++)
	{
		IMMDevice* d = nullptr; col->Item(i, &d);
		IPropertyStore* ps = nullptr; d->OpenPropertyStore(STGM_READ, &ps);
		PROPVARIANT v; PropVariantInit(&v); ps->GetValue(PKEY_Device_FriendlyName, &v);
		std::wstring name = v.pwszVal ? v.pwszVal : L"";
		PropVariantClear(&v); ps->Release();
		if (lower(name).find(want) != std::wstring::npos && !dev) { dev = d; chosen = name; }
		else d->Release();
	}
	if (!dev) { wprintf(L"dispositivo no encontrado\n"); return 3; }

	IAudioClient* ac = nullptr; dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&ac);
	WAVEFORMATEX* wf = nullptr; ac->GetMixFormat(&wf);
	if (FAILED(ac->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 10000000, 0, wf, nullptr))) { wprintf(L"no se pudo iniciar la captura\n"); return 4; }
	IAudioCaptureClient* cap = nullptr; ac->GetService(IID_PPV_ARGS(&cap));
	ac->Start();

	const int ch = wf->nChannels; const double sr = wf->nSamplesPerSec;
	bool isFloat = (wf->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) || (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE && wf->wBitsPerSample == 32);
	std::vector<double> sumsq(ch, 0.0); long frames = 0;
	std::vector<double> gc(freqs.size(), 0.0), gs(freqs.size(), 0.0);
	DWORD t0 = GetTickCount(); long idx = 0;
	while (GetTickCount() - t0 < (DWORD)(seconds * 1000))
	{
		Sleep(10);
		UINT32 pkt = 0; cap->GetNextPacketSize(&pkt);
		while (pkt)
		{
			BYTE* data; UINT32 nf; DWORD flags;
			cap->GetBuffer(&data, &nf, &flags, nullptr, nullptr);
			for (UINT32 i = 0; i < nf; i++, idx++)
			{
				double mono = 0;
				for (int c = 0; c < ch; c++)
				{
					double s = 0;
					if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT))
						s = isFloat ? ((float*)data)[i * ch + c] : ((short*)data)[i * ch + c] / 32768.0;
					sumsq[c] += s * s; mono += s;
				}
				mono /= ch; frames++;
				for (size_t k = 0; k < freqs.size(); k++) { double ph = 2 * 3.14159265358979 * freqs[k] * idx / sr; gc[k] += mono * std::cos(ph); gs[k] += mono * std::sin(ph); }
			}
			cap->ReleaseBuffer(nf); cap->GetNextPacketSize(&pkt);
		}
	}
	ac->Stop();
	wprintf(L"%s | %d ch %.0f Hz | ", chosen.c_str(), ch, sr);
	for (int c = 0; c < ch && c < 2; c++) wprintf(L"%s %.1f dBFS  ", c == 0 ? L"L" : L"R", 10 * std::log10(sumsq[c] / (frames ? frames : 1) + 1e-20));
	for (size_t k = 0; k < freqs.size(); k++)
	{
		double amp = 2.0 * std::sqrt(gc[k] * gc[k] + gs[k] * gs[k]) / (frames ? frames : 1);
		wprintf(L"| %.0f Hz %.1f dB ", freqs[k], 20 * std::log10(amp + 1e-12));
	}
	wprintf(L"\n");
	return 0;
}
