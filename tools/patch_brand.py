"""Cambios de marca y privacidad: carpeta de datos propia, sin llamadas a fxsound.com, sin updater.exe."""
import re, io, os
S = r'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/Source/'

def load(p):
    b = open(S + p, 'rb').read()
    bom = b.startswith(b'\xef\xbb\xbf')
    t = b.decode('utf-8-sig')
    nl = '\r\n' if '\r\n' in t else '\n'
    return t.replace('\r\n', '\n'), bom, nl

def save(p, t, bom, nl):
    t = t.replace('\n', nl)
    open(S + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.encode('utf-8'))

def edit(p, pairs, regex=()):
    t, bom, nl = load(p)
    for a, b in pairs:
        assert a in t, (p, a[:60])
        t = t.replace(a, b)
    for a, b in regex:
        t, n = re.subn(a, b, t)
        assert n, (p, a)
    save(p, t, bom, nl)

edit('Utils/Settings/Settings.h', [
    ('APPLICATION_NAME[] = L"FxSound"', 'APPLICATION_NAME[] = L"NubetaloSound"'),
    ('SETTINGS_FOLDER[] = L"FxSound"', 'SETTINGS_FOLDER[] = L"NubetaloSound"')])

edit('Main.cpp', [('getChildFile("FxSound")', 'getChildFile("NubetaloSound")')])

edit('GUI/FxController.cpp', [
    ('L"FxSoundHotkeys"', 'L"NubetaloSoundHotkeys"'),
    ('createDefaultAppLogger(L"FxSound", L"fxsound.log", L"FxSound logs")', 'createDefaultAppLogger(L"NubetaloSound", L"nubetalo.log", L"Nubetalo Sound logs")'),
    ('.getChildFile("FxSound").getChildFile("status.json")', '.getChildFile("NubetaloSound").getChildFile("status.json")'),
    # sin RegDeleteTree de claves de DFX/FxSound (pertenecen a otro programa) ni aviso con enlace a fxsound.com
    ('			RegDeleteTree(HKEY_CURRENT_USER, L"Software\\\\DFX");\n\n', ''),
    ('			FxModel::getModel().pushMessage(" ", { TRANS("Click here to see what\'s new on this version!"), "https://www.fxsound.com/changelog" });\n\n', ''),
    ('	if (auto_updates_ && current_time.getHours() == 10 && current_time.getMinutes() == 00 && current_time.getSeconds() == 0)\n	{\n		checkUpdates();\n	}\n', ''),
], regex=[(r'L"FxSound\\\\(Presets|AutoSave)', r'L"NubetaloSound\\\\\1')])

edit('GUI/FxController.cpp', [], regex=[
    (r'(?s)void FxController::checkUpdates\(\)\n\{.*?\n\}\n', 'void FxController::checkUpdates()\n{\n	// Nubetalo Sound no se actualiza solo ni abre procesos externos.\n}\n'),
])

# enlaces de ayuda -> correo de soporte
edit('GUI/FxController.cpp', [
    ('URL(L"https://www.fxsound.com/learning-center/installation-troubleshooting")', 'FxSupport::mailto("Problema con el dispositivo de audio")'),
    ('URL(L"https://www.fxsound.com/support")', 'FxSupport::mailto("Ayuda")'),
])
edit('GUI/FxView.cpp', [('"https://www.fxsound.com/learning-center/no-sound-with-fxsound-realtek"', 'FxSupport::mailto("Sin sonido por exclusivo").toString(true)')])

edit('MainComponent.cpp', [], regex=[(r'(?s)void MainComponent::showAnnouncement\(\)\n\{.*?\n\}\n', 'void MainComponent::showAnnouncement()\n{\n	// Desactivado: Nubetalo Sound no carga contenido remoto.\n}\n')])
edit('MainComponent.h', [], regex=[(r'constexpr wchar_t CHECK_ANNOUNCEMENT_URL\[\] = [^\n]*\n', '')])

edit('GUI/FxMainWindow.cpp', [], regex=[
    (r'\tauto downloadClicked = \[\]\(\) \{.*?\n\t\};\n\n', ''),
    (r'\tauto checkForUpdatesClicked = \[\]\(\) \{.*?\n\t\};\n\n', ''),
])
print('ok')

edit('GUI/FxController.cpp', [('survey_tip_ = !settings_.getBool("survey_displayed");', 'survey_tip_ = false; // sin encuestas externas'),
    ('#include "FxSystemTrayView.h"', '#include "FxSystemTrayView.h"\n#include "FxSupport.h"')])
edit('GUI/FxView.cpp', [], regex=[(r'(#include "FxView.h"\n)', r'\1#include "FxSupport.h"\n')])
print('ok2')
