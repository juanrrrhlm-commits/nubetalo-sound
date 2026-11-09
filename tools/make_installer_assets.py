"""Imágenes del asistente de instalación (164x314 y 55x58), en negro y azul con la marca abierta."""
import math, os
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'installer', 'assets')
os.makedirs(OUT, exist_ok=True)
RING, WAVE, BG1, BG2 = (47, 123, 255), (141, 191, 255), (7, 9, 13), (14, 22, 40)


def mark(size, tile=False):
    s = 8
    S = size * s
    im = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    u = S / 24.0
    if tile:
        d.rounded_rectangle([0, 0, S - 1, S - 1], radius=int(5.4 * u), fill=(7, 9, 13, 255), outline=(26, 42, 69, 255), width=max(1, int(0.7 * u)))
    r = 8.2 * u
    box = [12 * u - r, 12 * u - r, 12 * u + r, 12 * u + r]
    d.arc(box, start=302, end=238, fill=RING, width=int(2.2 * u))   # abierto arriba
    pts = []
    for i in range(0, 61):
        t = i / 60.0
        x = 7.4 + t * 9.2
        y = 12.6 - 3.0 * math.sin(t * 2 * math.pi) * 1.0
        pts.append((x * u, y * u))
    d.line(pts, fill=WAVE, width=int(1.7 * u), joint='curve')
    return im.resize((size, size), Image.LANCZOS)


def gradient(w, h):
    im = Image.new('RGB', (w, h), BG1)
    px = im.load()
    for y in range(h):
        for x in range(w):
            t = y / h
            px[x, y] = tuple(int(BG1[i] + (BG2[i] - BG1[i]) * t) for i in range(3))
    return im


big = gradient(164, 314)
m = mark(110, tile=False)
big.paste(m, (27, 70), m)
ImageDraw.Draw(big).rectangle([0, 311, 164, 314], fill=RING)
big.save(os.path.join(OUT, 'wizard_big.bmp'))

small = Image.new('RGB', (55, 58), BG1)
m2 = mark(40, tile=True)
small.paste(m2, (7, 9), m2)
small.save(os.path.join(OUT, 'wizard_small.bmp'))
print('imágenes listas')
