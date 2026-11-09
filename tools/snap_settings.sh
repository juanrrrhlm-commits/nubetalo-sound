#!/bin/bash
# Compila y guarda una captura de la pantalla de Configuración en tools/snap_settings.png
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
taskkill //F //IM NubetaloSound.exe >/dev/null 2>&1
"$ROOT/tools/build.sh" || exit 1
rm -f "$ROOT/tools/snap_settings.png"
cd "$ROOT/app/bin/x64" && FXSOUND_SETTINGS="$(cygpath -w "$ROOT/tools/snap_settings.png")" timeout 25 "./NubetaloSound.exe"
ls -la "$ROOT/tools/snap_settings.png"
