// Datos de contacto y ayuda de Nubetalo Sound. Todo el soporte llega por correo; la app no abre
// ninguna otra conexión por su cuenta.
#pragma once

#include <JuceHeader.h>

namespace FxSupport
{
	inline String email() { return "juanrrrhlm@gmail.com"; }

	// Etiqueta que lleva todo asunto, para filtrarlo en la bandeja (Gmail: etiqueta "Nubetalo Sound Soporte").
	inline String tag() { return "[Nubetalo Sound Soporte]"; }

	inline String versionText()
	{
		auto* app = JUCEApplication::getInstance();
		return app != nullptr ? app->getApplicationVersion() : String("1.0.0");
	}

	// Datos útiles para el soporte; no incluye nombres de dispositivos ni rutas personales.
	inline String systemInfo()
	{
		String s;
		s << "Nubetalo Sound " << versionText() << "\n"
		  << SystemStats::getOperatingSystemName() << (SystemStats::isOperatingSystem64Bit() ? " (64 bits)" : " (32 bits)") << "\n"
		  << "Idioma: " << SystemStats::getDisplayLanguage() << "\n";
		return s;
	}

	inline URL mailto(const String& subject, bool with_system_info = false)
	{
		String u = "mailto:" + email() + "?subject=" + URL::addEscapeChars(tag() + " " + subject, false);
		if (with_system_info)
			u << "&body=" << URL::addEscapeChars("\n\n---\n" + systemInfo().replace("\n", "\r\n"), false);
		return URL(u);
	}

	inline void openMail(const String& subject, bool with_system_info = false)
	{
		mailto(subject, with_system_info).launchInDefaultBrowser();
	}
}
