"""Busca en el ejecutable restos que no deben distribuirse (ganchos de desarrollo, rutas del equipo, URLs heredadas)."""
import sys
d = open(sys.argv[1], 'rb').read()
bad = 0
for s in ['FXSOUND_SNAPSHOT', 'FXSOUND_PRESETIO', 'FXSOUND_DEMO', 'FXSOUND_FLIP', 'juanr', 'fxsound-mejorado', 'fxsound.com', 'updater.exe', 'C:' + chr(92) + 'Users']:
    n = d.count(s.encode()) + d.count(s.encode('utf-16le'))
    print(f'{s:22} {n}')
    bad += n
print('LIMPIO' if bad == 0 else f'{bad} coincidencias')
