// Aviso de versión nueva. Consulta una sola vez la última release pública en GitHub (solo lectura, sin datos del usuario)
// y avisa si hay una versión mayor. No descarga ni ejecuta nada: abre la página de descarga en el navegador.
// Copyright (C) 2026 Nubetalo. Licencia AGPL v3.
#pragma once

#include <JuceHeader.h>

namespace FxUpdater
{
	inline String repoUrl() { return "https://github.com/juanrrrhlm-commits/nubetalo-sound"; }
	inline String apiUrl()  { return "https://api.github.com/repos/juanrrrhlm-commits/nubetalo-sound/releases/latest"; }

	// "v1.2.3" o "1.2.3" -> {1,2,3}; vacío si no es una versión válida.
	inline Array<int> parseVersion(String v)
	{
		Array<int> out;
		v = v.trim();
		if (v.startsWithIgnoreCase("v")) v = v.substring(1);
		if (v.isEmpty() || v.length() > 20) return out;
		for (auto& part : StringArray::fromTokens(v, ".", ""))
		{
			if (part.isEmpty() || !part.containsOnly("0123456789") || part.length() > 6) return {};
			out.add(part.getIntValue());
		}
		return out.size() >= 1 && out.size() <= 4 ? out : Array<int>();
	}

	inline bool isNewer(const String& remote, const String& local)
	{
		auto r = parseVersion(remote), l = parseVersion(local);
		if (r.isEmpty() || l.isEmpty()) return false;
		for (int i = 0; i < 4; ++i)
		{
			const int a = i < r.size() ? r[i] : 0, b = i < l.size() ? l[i] : 0;
			if (a != b) return a > b;
		}
		return false;
	}

	struct Result
	{
		bool ok = false;          // la consulta funcionó
		bool newer = false;       // hay versión mayor
		String version;           // "1.0.1"
		String url;               // página de descarga (siempre dentro del repositorio del proyecto)
	};

	// Se ejecuta en un hilo aparte; devuelve el resultado en el hilo de mensajes.
	inline void check(const String& current, std::function<void(Result)> done)
	{
		Thread::launch([current, done]() {
			Result res;
			auto options = URL::InputStreamOptions(URL::ParameterHandling::inAddress)
				.withConnectionTimeoutMs(8000)
				.withExtraHeaders("User-Agent: NubetaloSound/" + current + "\r\nAccept: application/vnd.github+json");
			if (auto stream = URL(apiUrl()).createInputStream(options))
			{
				MemoryBlock block;
				stream->readIntoMemoryBlock(block, 262144);   // tope de 256 KB
				auto json = JSON::parse(block.toString());
				auto tag = json.getProperty("tag_name", {}).toString();
				if (parseVersion(tag).size() > 0)
				{
					res.ok = true;
					res.version = tag.startsWithIgnoreCase("v") ? tag.substring(1) : tag;
					res.newer = isNewer(tag, current);
					res.url = repoUrl() + "/releases/latest";
					// enlace directo al instalador, solo si pertenece al repositorio del proyecto
					if (auto* assets = json.getProperty("assets", {}).getArray())
						for (auto& a : *assets)
						{
							auto name = a.getProperty("name", {}).toString();
							auto dl = a.getProperty("browser_download_url", {}).toString();
							if (name.endsWithIgnoreCase(".exe") && dl.startsWith(repoUrl() + "/releases/download/"))
							{
								res.url = dl;
								break;
							}
						}
				}
			}
			MessageManager::callAsync([done, res]() { done(res); });
		});
	}
}
