"""Portada (630x500) y capturas para la página de itch.io. Uso: python make_itch_assets.py <carpeta de salida>"""
import os, sys
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMG = os.path.join(ROOT, 'docs', 'img')
F = os.path.join(ROOT, 'app', 'fxsound', 'Fonts')
out = sys.argv[1]
W, H = 630, 500
im = Image.new('RGB', (W, H), (7, 9, 13))
px = im.load()
for y in range(H):
    for x in range(W):
        t = min(1, (x / W) * 0.3 + (y / H) * 0.7)
        px[x, y] = (int(7 + 7 * t), int(9 + 15 * t), int(13 + 33 * t))
logo = Image.open(os.path.join(IMG, 'logo-512.png')).convert('RGBA').resize((96, 96), Image.LANCZOS)
im.paste(logo, (267, 40), logo)
d = ImageDraw.Draw(im)
d.text((W / 2, 172), 'Nubetalo Sound', font=ImageFont.truetype(os.path.join(F, 'IBMPlexSans-SemiBold.ttf'), 46), fill=(244, 247, 255), anchor='mm')
d.text((W / 2, 214), 'Mejorador de audio para Windows', font=ImageFont.truetype(os.path.join(F, 'IBMPlexSans-Regular.ttf'), 21), fill=(170, 190, 224), anchor='mm')
s = Image.open(os.path.join(IMG, 'principal.png')).convert('RGBA')
s = s.resize((420, int(s.height * 420 / s.width)), Image.LANCZOS)
im.paste(s, ((W - 420) // 2, 250), s)
im.save(os.path.join(out, 'cover.png'))
for n in ('principal', 'ajustes1', 'ajustes0', 'menu'):
    Image.open(os.path.join(IMG, n + '.png')).convert('RGB').save(os.path.join(out, 'shot_%s.png' % n), optimize=True)
print(sorted(os.listdir(out)))
