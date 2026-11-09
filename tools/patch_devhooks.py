"""Los ganchos de desarrollo (variables FXSOUND_*) solo existen si NO se define NUBETALO_RELEASE."""
G = 'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/Source/GUI/'

def rw(p, fn):
    raw = open(G + p, 'rb').read()
    bom = raw.startswith(b'\xef\xbb\xbf')
    nl = '\r\n' if b'\r\n' in raw else '\n'
    t = fn(raw.decode('utf-8-sig').replace('\r\n', '\n'))
    open(G + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.replace('\n', nl).encode('utf-8'))

def main_window(t):
    a = '	// Developer aid: FXSOUND_MENU=<file.png>'
    assert t.count(a) == 1
    t = t.replace(a, '#ifndef NUBETALO_RELEASE\n' + a)
    b = '	}\n}\n\nFxMainWindow::~FxMainWindow()'
    assert t.count(b) == 1
    t = t.replace(b, '	}\n#endif // NUBETALO_RELEASE\n}\n\nFxMainWindow::~FxMainWindow()')
    # el registro del gancho de presets ya no apunta a una ruta personal
    t = t.replace('File(File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "\\\\fxsound-mejorado\\\\tools\\\\presetio.log").appendText(log);',
                  'File::getSpecialLocation(File::tempDirectory).getChildFile("nubetalo_presetio.log").appendText(log);')
    return t

def audio_controls(t):
    a = '	// Developer aid: FXSOUND_FLIP=1'
    assert t.count(a) == 1
    i = t.index(a)
    j = t.index('\n	}\n', i) + len('\n	}\n')
    return t[:i] + '#ifndef NUBETALO_RELEASE\n' + t[i:j] + '#endif\n' + t[j:]

def visualizer(t):
    a = '    static const bool demo = SystemStats::getEnvironmentVariable("FXSOUND_DEMO", "") == "1";'
    assert t.count(a) == 1
    return t.replace(a, '#ifndef NUBETALO_RELEASE\n' + a + '\n#else\n    static const bool demo = false;\n#endif')

rw('FxMainWindow.cpp', main_window)
rw('FxAudioControls.cpp', audio_controls)
rw('FxVisualizer.cpp', visualizer)
print('ok')
