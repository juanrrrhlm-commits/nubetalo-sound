# CÃ³mo publicar Nubetalo Sound 1.0.1

1. Crear el repositorio pÃºblico en GitHub (ej. `Nubetalo/nubetalo-sound`) y subir esta carpeta (`git remote add origin ...`, `git push -u origin main`). JUCE no se sube: se descarga aparte (ver README).
2. Crear una *Release* con la etiqueta `v1.0.1` y adjuntar:
   - `installer/output/NubetaloSound-Setup-1.0.1.exe`
   - SHA-256: `89028cd5cc4d1919dec574d0a286ca98ddaac1a84a3ead7258697ca0c2f8e2da`
3. En la descripciÃ³n de la release: enlace al cÃ³digo fuente (obligatorio por la AGPL), aviso de que el instalador no estÃ¡ firmado (SmartScreen puede avisar: "MÃ¡s informaciÃ³n" > "Ejecutar de todos modos") y que el dispositivo virtual de audio es de FxSound.
4. Antes de cada versiÃ³n nueva: `tools\build_release.sh`, `python tools\scan_binary.py app\bin\x64\NubetaloSound.exe`, `tools\make_installer.ps1`, pruebas de `tools\dsp_test`.

## Lo que evita problemas mÃ¡s adelante
- Licencia: AGPL v3 intacta; crÃ©ditos a FxSound, JUCE e IBM Plex en NOTICE.txt. No usar el nombre ni el logo de FxSound.
- Todo cambio futuro en cÃ³digo derivado se publica bajo AGPL.
- Quien contribuya acepta que su aporte entra bajo AGPL.
- Soporte y privacidad: PRIVACY.md y SECURITY.md; el correo de soporte es pÃºblico.
- Firma de cÃ³digo (opcional, mejora SmartScreen): certificado OV/EV o SignPath gratis para proyectos de cÃ³digo abierto.


