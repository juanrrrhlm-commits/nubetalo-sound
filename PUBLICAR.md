# Cómo publicar Nubetalo Sound 1.0.0

1. Crear el repositorio público en GitHub (ej. `Nubetalo/nubetalo-sound`) y subir esta carpeta (`git remote add origin ...`, `git push -u origin main`). JUCE no se sube: se descarga aparte (ver README).
2. Crear una *Release* con la etiqueta `v1.0.0` y adjuntar:
   - `installer/output/NubetaloSound-Setup-1.0.0.exe`
   - SHA-256: `5909c7c2e6f2fb56c2ee2ebfe15dd6858703a32ca3e0437037f7cbc2fd32944f`
3. En la descripción de la release: enlace al código fuente (obligatorio por la AGPL), aviso de que el instalador no está firmado (SmartScreen puede avisar: "Más información" > "Ejecutar de todos modos") y que el dispositivo virtual de audio es de FxSound.
4. Antes de cada versión nueva: `tools\build_release.sh`, `python tools\scan_binary.py app\bin\x64\NubetaloSound.exe`, `tools\make_installer.ps1`, pruebas de `tools\dsp_test`.

## Lo que evita problemas más adelante
- Licencia: AGPL v3 intacta; créditos a FxSound, JUCE e IBM Plex en NOTICE.txt. No usar el nombre ni el logo de FxSound.
- Todo cambio futuro en código derivado se publica bajo AGPL.
- Quien contribuya acepta que su aporte entra bajo AGPL.
- Soporte y privacidad: PRIVACY.md y SECURITY.md; el correo de soporte es público.
- Firma de código (opcional, mejora SmartScreen): certificado OV/EV o SignPath gratis para proyectos de código abierto.
