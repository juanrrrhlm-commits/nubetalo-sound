#pragma once
#include <JuceHeader.h>

// Único lugar donde se configura la donación.
// Enlace de donación (PayPal.Me). Si se deja vacío, el botón y los menús de donar se ocultan.
namespace FxDonate
{
	inline String url()
	{
		return "https://www.paypal.me/Nubetalo";
	}

	inline bool enabled()
	{
		return url().isNotEmpty();
	}

	inline void open()
	{
		if (enabled())
			URL(url()).launchInDefaultBrowser();
	}
}
