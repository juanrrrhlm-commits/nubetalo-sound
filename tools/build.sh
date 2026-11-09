#!/bin/bash
# Compila FxSound (Release x64). Uso: tools/build.sh
MSB="/c/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/MSBuild/Current/Bin/MSBuild.exe"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/app/fxsound/Project" || exit 1
"$MSB" "Nubetalo Sound_App.vcxproj" -p:Configuration=Release -p:Platform=x64 -m -v:minimal > "$ROOT/fxsound_build.log" 2>&1
code=$?
grep -E "error " "$ROOT/fxsound_build.log" | sort -u | head -30 | cut -c1-300
echo "exit=$code"
