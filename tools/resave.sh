#!/bin/bash
# Regenera BinaryData (imágenes) y el proyecto de VS con Projucer. Usar tras cambiar archivos en fxsound/Images.
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
"$ROOT/JUCE/extras/Projucer/Builds/VisualStudio2022/x64/Release/App/Projucer.exe" --resave "$ROOT/app/fxsound/FxSound.jucer" | tail -3
python -I "$ROOT/tools/patch_refs.py" "$ROOT/app/fxsound/Project/Nubetalo Sound_App.vcxproj"
