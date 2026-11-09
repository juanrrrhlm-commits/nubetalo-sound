# Genera los presets propios (6, generales) en el formato de texto .fac del motor.
import os, shutil
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'app')
FREQS = [62.5, 115.734, 214.311, 396.85, 734.867, 1360.79, 2519.84, 4666.12, 8640.48, 13600]
# nombre: (claridad, envolvente, ambiente, refuerzo dinamico, graves, [10 bandas dB])
PRESETS = [
 ('Equilibrado', (2,   0, 0, 2, 1.5, [2, 1, 0, 0, 0, 0, 0, 1, 2, 2])),
 ('Plano',       (0,   0, 0, 0, 0,   [0]*10)),
 ('Voces',       (3,   0, 0, 3, 0,   [-3, -2, -1, 1, 2, 3, 3, 2, 0, -1])),
 ('Graves',      (1.5, 0, 0, 3, 6,   [5, 4, 3, 1, 0, 0, 0, 0, 1, 1])),
 ('Cine',        (2,   4, 2, 3, 3,   [3, 2, 1, 0, -1, 0, 1, 2, 3, 3])),
 ('Gamer',       (4,   5, 1, 4, 3,   [2, 1, 0, -1, -1, 0, 2, 4, 3, 1])),
 ('Noche',       (1,   0, 0, 7, 2,   [1, 1, 0, 0, 0, -1, -2, -2, -3, -3])),
]
def fac(name, v):
    clar, surr, amb, dyn, bass, eq = v
    m = lambda x: int(round(x * 12.5))
    L = ['CLASS1 : Effect Type', '9: Version', name, '0: Double Params Flag', '1: Total number of elements',
         f'{m(clar)}: Main 0', f'{m(surr)}: Main 1', '0: Main 2', f'{m(amb)}: Main 3', f'{m(dyn)}: Main 4', f'{m(bass)}: Main 5',
         '0: Element Number'] + [f'   0: Param {i}' for i in range(7)] + [
         '7: Number of Application Dependent Integers', '0: Number of Application Dependent Reals', '0: Number of Application Dependent Strings',
         '1: Integer[0]', '1: Integer[1]', '0: Integer[2]', '1: Integer[3]', '1: Integer[4]', '0: Integer[5]', '2: Integer[6]',
         '10: Number of EQ Bands', '1: On/Off Flag']
    for i, (f, g) in enumerate(zip(FREQS, eq)):
        L += [f'Band {i+1}', f'   {f:g}: CF', f'   {g:g}: Boost/Cut']
    return '\r\n'.join(L)
for arch in ('x64', 'arm64', 'x86'):
    d = os.path.join(ROOT, 'bin', arch, 'Factsoft')
    if not os.path.isdir(d): continue
    for f in os.listdir(d):
        if f.endswith('.fac'): os.remove(os.path.join(d, f))
    for i, (n, v) in enumerate(PRESETS, 1):
        open(os.path.join(d, f'{i}.fac'), 'w', newline='').write(fac(n, v))
# materiales de presets del FxSound original: se quitan de esta copia
for extra in (os.path.join(ROOT, 'bin', 'BonusPresets'), os.path.join(ROOT, 'Resources', 'Presets')):
    shutil.rmtree(extra, ignore_errors=True)
print('presets listos:', [n for n, _ in PRESETS])
