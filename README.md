# Nubetalo Sound

Mejorador de audio de código abierto para Windows. Ecualizador de 10 bandas, efectos (claridad, ambiente, sonido envolvente, refuerzo dinámico y de graves), potencia ajustable de +6 a +30 dB, espectro en tiempo real y preajustes por dispositivo.

Derivado de [FxSound](https://github.com/fxsound2/fxsound-app) (AGPL v3). Interfaz, iconos, tipografía y presets propios; corrige la pérdida de sonido al cambiar de altavoces a auriculares.

## Instalación
Ejecuta `NubetaloSound-Setup-1.0.0.exe`. Instala el programa, los presets y, si no existe, el dispositivo de audio virtual necesario (pide permisos de administrador).

## Uso rápido
- Interruptor de la barra: enciende o apaga el procesado.
- Menú de tres puntos: ajustes, guardar/sobrescribir/renombrar presets, exportar e importar, siempre arriba, donar.
- Atajos (editables en Ajustes › General): `Ctrl+Alt+N` encender/apagar, `Ctrl+Alt+V` abrir/cerrar, `Ctrl+Alt+P` / `Ctrl+Alt+Mayús+P` preajuste siguiente/anterior, `Ctrl+Alt+D` cambiar dispositivo.
- Línea de comandos: `NubetaloSound.exe --power=1 --preset=Gamer` (ver `docs/COMMAND_LINE_OPTIONS.md`).

## Compilar
Requisitos: Visual Studio 2022 Build Tools (C++), Windows SDK, JUCE 6.1.6 en `JUCE\`.
```
tools\resave.sh            # genera el proyecto con Projucer
tools\build_release.sh     # versión de distribución (sin ganchos de desarrollo)
```

## Documentos
- [LICENSE.txt](LICENSE.txt) · [NOTICE.txt](NOTICE.txt) (avisos y créditos)
- [PRIVACY.md](PRIVACY.md) · [SECURITY.md](SECURITY.md)

## Soporte y donaciones
Correo: juanrrrhlm@gmail.com (asunto `[Nubetalo Sound Soporte] ...`) · Donar: https://www.paypal.me/Nubetalo
