"""Imágenes de presentación: logo 512 (avatar) y banner 1280x640 (portada de GitHub / itch.io)."""
import math, os, sys
from PIL import Image, ImageDraw, ImageFont, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMG = os.path.join(ROOT, 'docs', 'img')
FONTS = os.path.join(ROOT, 'app', 'fxsound', 'Fonts')
RING, WAVE = (47, 123, 255), (141, 191, 255)
BG1, BG2 = (7, 9, 13), (14, 24, 46)


def mark(size, tile=True):
    s = 4
    S = size * s
    im = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    u = S / 24.0
    if tile:
        d.rounded_rectangle([0, 0, S - 1, S - 1], radius=int(5.4 * u), fill=(7, 9, 13, 255), outline=(26, 42, 69, 255), width=max(1, int(0.7 * u)))
    r = 8.2 * u
    d.arc([12 * u - r, 12 * u - r, 12 * u + r, 12 * u + r], start=302, end=238, fill=RING, width=int(2.2 * u))
    pts = [((7.4 + (i / 60) * 9.2) * u, (12.6 - 3.0 * math.sin((i / 60) * 2 * math.pi)) * u) for i in range(61)]
    d.line(pts, fill=WAVE, width=int(1.7 * u), joint='curve')
    return im.resize((size, size), Image.LANCZOS)


def font(name, size):
    return ImageFont.truetype(os.path.join(FONTS, name), size)


def gradient(w, h):
    im = Image.new('RGB', (w, h), BG1)
    px = im.load()
    for y in range(h):
        for x in range(w):
            t = min(1.0, (x / w) * 0.6 + (y / h) * 0.4)
            px[x, y] = tuple(int(BG1[i] + (BG2[i] - BG1[i]) * t) for i in range(3))
    return im


# avatar / logo
mark(512).save(os.path.join(IMG, 'logo-512.png'))

# banner
W, H = 1280, 640
banner = gradient(W, H)
glow = Image.new('RGBA', (W, H), (0, 0, 0, 0))
ImageDraw.Draw(glow).ellipse([700, 60, 1380, 700], fill=(47, 123, 255, 70))
banner = Image.alpha_composite(banner.convert('RGBA'), glow.filter(ImageFilter.GaussianBlur(90)))

shot = Image.open(os.path.join(IMG, 'principal.png')).convert('RGBA')
sw = 640
shot = shot.resize((sw, int(shot.height * sw / shot.width)), Image.LANCZOS)
shadow = Image.new('RGBA', (shot.width + 80, shot.height + 80), (0, 0, 0, 0))
ImageDraw.Draw(shadow).rounded_rectangle([40, 40, shot.width + 40, shot.height + 40], radius=20, fill=(0, 0, 0, 190))
shadow = shadow.filter(ImageFilter.GaussianBlur(22))
sx, sy = 590, (H - shot.height) // 2 + 14
banner.alpha_composite(shadow, (sx - 40, sy - 40))
banner.alpha_composite(shot, (sx, sy))

banner.alpha_composite(mark(84), (70, 120))
d = ImageDraw.Draw(banner)
d.text((70, 232), 'Nubetalo Sound', font=font('IBMPlexSans-SemiBold.ttf', 66), fill=(244, 247, 255))
d.text((72, 322), 'Más graves, más claridad,\nmás volumen. Gratis y\nde código abierto.', font=font('IBMPlexSans-Regular.ttf', 30), fill=(170, 190, 224), spacing=10)
d.rounded_rectangle([72, 472, 340, 522], radius=25, outline=RING, width=2)
d.text((206, 497), 'Para Windows 10 / 11', font=font('IBMPlexSans-Medium.ttf', 22), fill=(141, 191, 255), anchor='mm')
banner.convert('RGB').save(os.path.join(IMG, 'banner.png'))
print('logo-512.png y banner.png listos')
