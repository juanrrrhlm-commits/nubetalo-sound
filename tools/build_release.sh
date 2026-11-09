#!/bin/bash
# Compilación de distribución: sin ganchos de desarrollo (NUBETALO_RELEASE) y sin rutas del equipo dentro del binario.
# Salida: app/bin/x64/NubetaloSound.exe
MSB="/c/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/MSBuild/Current/Bin/MSBuild.exe"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WROOT="$(cygpath -w "$ROOT")"
export CL="/DNUBETALO_RELEASE /d1trimfile:${WROOT}\\"
cd "$ROOT/app/fxsound/Project" || exit 1
"$MSB" "Nubetalo Sound_App.vcxproj" -t:Rebuild -p:Configuration=Release -p:Platform=x64 -m -v:minimal > "$ROOT/release_build.log" 2>&1
code=$?
unset CL
grep -E "error " "$ROOT/release_build.log" | sort -u | head -20 | cut -c1-300
echo "exit=$code"
