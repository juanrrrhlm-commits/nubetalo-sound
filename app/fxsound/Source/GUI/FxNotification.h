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

#ifndef FXNOTIFICATION_H
#define FXNOTIFICATION_H

#include <JuceHeader.h>
#include "FxTheme.h"
#include "FxController.h"

// Aviso en forma de píldora (estilo "isla"): aparece arriba a la izquierda de la pantalla, se expande,
// se queda unos segundos y se contrae. Con autohide = false se dibuja como tarjeta dentro de otra vista.
class FxNotification : public Component, private Timer
{
public:
	FxNotification();
	~FxNotification();

	static constexpr int WIDTH = 216;
	static constexpr int HEIGHT = 80;
	static constexpr int MAX_WIDTH = 560;
	static constexpr int MAX_HEIGHT = 120;
	static constexpr int MARGIN = 10;   // espacio transparente alrededor, para el resplandor
	static constexpr int PILL_HEIGHT = 36;

	void setMessage(const String& message, const std::pair<String, String>& link, bool autohide = true);
	void showMessage(bool autohide = true);

private:
	enum class Phase { Hidden, Opening, Holding, Closing };

	void paint(Graphics&) override;
	void mouseUp(const MouseEvent&) override;
	void timerCallback() override;
	void close();

	String text_;
	std::pair<String, String> link_;
	bool autohide_ = true;
	Phase phase_ = Phase::Hidden;
	float progress_ = 1.0f;   // 0 = cápsula mínima, 1 = píldora completa
	int hold_ticks_ = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxNotification)
};

#endif
