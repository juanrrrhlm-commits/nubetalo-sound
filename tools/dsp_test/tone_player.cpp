// Reproductor WASAPI mínimo (modo compartido, dispositivo predeterminado) que registra errores.
// Uso: tone_player.exe <segundos> [reabrir]
//  - sin "reabrir": no se recupera si el dispositivo se invalida (como una app simple).
//  - con "reabrir": vuelve a abrir el dispositivo predeterminado si recibe AUDCLNT_E_DEVICE_INVALIDATED (como un navegador).
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <cmath>
#include <cstdio>

static IAudioClient* ac = nullptr;
static IAudioRenderClient* rc = nullptr;
static WAVEFORMATEX* wf = nullptr;
static UINT32 bufFrames = 0;
static double phase = 0;

static bool openDefault()
{
	if (rc) { rc->Release(); rc = nullptr; }
	if (ac) { ac->Stop(); ac->Release(); ac = nullptr; }
	IMMDeviceEnumerator* en = nullptr;
	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&en));
	IMMDevice* d = nullptr;
	if (FAILED(en->GetDefaultAudioEndpoint(eRender, eConsole, &d))) return false;
	d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&ac);
	ac->GetMixFormat(&wf);
	if (FAILED(ac->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 2000000, 0, wf, nullptr))) return false;
	ac->GetBufferSize(&bufFrames);
	ac->GetService(IID_PPV_ARGS(&rc));
	// pre-llenar
	BYTE* p; rc->GetBuffer(bufFrames, &p); memset(p, 0, bufFrames * wf->nBlockAlign); rc->ReleaseBuffer(bufFrames, 0);
	ac->Start();
	return true;
}

int wmain(int argc, wchar_t** argv)
{
	double seconds = argc > 1 ? _wtof(argv[1]) : 30;
	bool reopen = argc > 2;
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (!openDefault()) { printf("no se pudo abrir\n"); return 1; }
	printf("abierto: %d ch %lu Hz, buffer %u\n", wf->nChannels, wf->nSamplesPerSec, bufFrames);
	DWORD t0 = GetTickCount(), lastLog = t0; long written = 0; HRESULT lastErr = S_OK; int errors = 0, reopens = 0;
	while (GetTickCount() - t0 < (DWORD)(seconds * 1000))
	{
		Sleep(10);
		UINT32 pad = 0; HRESULT hr = ac->GetCurrentPadding(&pad);
		if (SUCCEEDED(hr))
		{
			UINT32 n = bufFrames - pad; BYTE* p = nullptr;
			if (n > 0 && SUCCEEDED(hr = rc->GetBuffer(n, &p)))
			{
				float* f = (float*)p;
				for (UINT32 i = 0; i < n; i++)
				{
					float s = 0.1f * (float)std::sin(phase); phase += 2 * 3.14159265358979 * 440.0 / wf->nSamplesPerSec;
					for (int c = 0; c < wf->nChannels; c++) f[i * wf->nChannels + c] = s;
				}
				rc->ReleaseBuffer(n, 0); written += n;
			}
		}
		if (FAILED(hr))
		{
			if (hr != lastErr) { printf("[%5.1fs] error 0x%08lx\n", (GetTickCount() - t0) / 1000.0, hr); lastErr = hr; }
			errors++;
			if (reopen && hr == AUDCLNT_E_DEVICE_INVALIDATED) { Sleep(200); if (openDefault()) { reopens++; printf("[%5.1fs] reabierto\n", (GetTickCount() - t0) / 1000.0); lastErr = S_OK; } }
		}
		if (GetTickCount() - lastLog >= 2000) { lastLog = GetTickCount(); printf("[%5.1fs] escritos=%ld errores=%d reaperturas=%d\n", (GetTickCount() - t0) / 1000.0, written, errors, reopens); }
	}
	printf("fin: escritos=%ld errores=%d\n", written, errors);
	return 0;
}
