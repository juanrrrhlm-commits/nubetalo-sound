#!/bin/bash
# Compila y captura las tres pestañas de Configuración en tools/snap_tab{0,1,2}.png
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
taskkill //F //IM NubetaloSound.exe >/dev/null 2>&1
"$ROOT/tools/build.sh" || exit 1
cd "$ROOT/app/bin/x64"
for t in 0 1 2; do
  rm -f "$ROOT/tools/snap_tab$t.png"
  FXSOUND_TAB=$t FXSOUND_SETTINGS="$(cygpath -w "$ROOT/tools/snap_tab$t.png")" timeout 25 ./NubetaloSound.exe
done
ls "$ROOT"/tools/snap_tab*.png
