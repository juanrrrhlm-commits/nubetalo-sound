G = 'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/'

def rw(p, fn):
    raw = open(G + p, 'rb').read()
    bom = raw.startswith(b'\xef\xbb\xbf')
    nl = '\r\n' if b'\r\n' in raw else '\n'
    t = fn(raw.decode('utf-8-sig').replace('\r\n', '\n'))
    open(G + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.replace('\n', nl).encode('utf-8'))

def tray(t):
    a = '''            Point<int> pos = getSystemTrayWindowPosition(notification_.getWidth(), notification_.getHeight());
            notification_.setBounds(pos.x, pos.y, notification_.getWidth(), notification_.getHeight());'''
    assert a in t
    return t.replace(a, '''            // Arriba a la izquierda de la pantalla principal; el margen transparente de la píldora queda fuera del borde visible.
            const auto area = Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
            notification_.setBounds(area.getX() + 18 - FxNotification::MARGIN, area.getY() + 18 - FxNotification::MARGIN, notification_.getWidth(), notification_.getHeight());''')

rw('Source/GUI/FxSystemTrayView.cpp', tray)
rw('FxSound.jucer', lambda t: t.replace(r'path="C:\Users\juanr\Documents\fxsound-mejorado\JUCE\modules"', r'path="..\..\JUCE\modules"'))
print('ok')
