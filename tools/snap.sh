#!/bin/bash
# Compila y guarda una captura interna de la ventana en tools/snap.png (cierra cualquier FxSound abierto).
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
taskkill //F //IM "NubetaloSound.exe" >/dev/null 2>&1
"$ROOT/tools/build.sh" || exit 1
rm -f "$ROOT/tools/snap.png"
cd "$ROOT/app/bin/x64" && FXSOUND_SNAPSHOT="$(cygpath -w "$ROOT/tools/snap.png")" timeout 25 "./NubetaloSound.exe"
ls -la "$ROOT/tools/snap.png"
