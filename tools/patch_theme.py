G = 'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/Source/GUI/'

def load(p):
    b = open(G + p, 'rb').read()
    return b.decode('utf-8-sig').replace('\r\n', '\n'), b.startswith(b'\xef\xbb\xbf')

def save(p, t, bom):
    open(G + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.replace('\n', '\r\n').encode('utf-8'))

# ---- FxTheme.h: declaración
h, bom = load('FxTheme.h')
a = '    void loadFont('
if 'fontFamilyForLanguage' not in h:
  assert a in h, 'loadFont decl'
  h = h.replace(a, '    // Familia de Windows con los glifos de ese idioma (vacía si basta IBM Plex Sans).\n	static String fontFamilyForLanguage(const String& language_code);\n' + a, 1)
save('FxTheme.h', h, bom)

# ---- FxTheme.cpp: loadFont con tipografías del sistema
c, bom = load('FxTheme.cpp')
i = c.index('void FxTheme::loadFont(String language)')
j = c.index('    if (font_400_ == nullptr)', i)
new = '''String FxTheme::fontFamilyForLanguage(const String& l)
{
	if (l.startsWithIgnoreCase("ja")) return "Yu Gothic UI";
	if (l.startsWithIgnoreCase("ko")) return "Malgun Gothic";
	if (l.startsWithIgnoreCase("zh-TW")) return "Microsoft JhengHei UI";
	if (l.startsWithIgnoreCase("zh")) return "Microsoft YaHei UI";
	if (l.startsWithIgnoreCase("th")) return "Leelawadee UI";
	if (l.startsWithIgnoreCase("ar") || l.startsWithIgnoreCase("fa")) return "Segoe UI";
	return String();
}

void FxTheme::loadFont(String language)
{
	const auto family = fontFamilyForLanguage(language);
	if (family.isNotEmpty())
	{
		font_400_ = Typeface::createSystemTypefaceFor(Font(family, 14.0f, Font::plain));
		font_600_ = Typeface::createSystemTypefaceFor(Font(family, 14.0f, Font::plain));
		font_700_ = Typeface::createSystemTypefaceFor(Font(family, 14.0f, Font::bold));
	}
	else
	{
		font_400_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansRegular_ttf, BinaryData::IBMPlexSansRegular_ttfSize);
		font_600_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansMedium_ttf, BinaryData::IBMPlexSansMedium_ttfSize);
		font_700_ = Typeface::createSystemTypefaceFor(BinaryData::IBMPlexSansSemiBold_ttf, BinaryData::IBMPlexSansSemiBold_ttfSize);
	}

'''
c = c[:i] + new + c[j:]
save('FxTheme.cpp', c, bom)

# ---- FxSettingsDialog.cpp: helper sin getDefaultUiFont
s, bom = load('FxSettingsDialog.cpp')
a = '''		return family.isEmpty() ? FxTheme::getDefaultUiFont(height) : Font(family, height, Font::plain);'''
assert a in s
s = s.replace(a, '''		if (family.isEmpty())
			return dynamic_cast<FxTheme&>(LookAndFeel::getDefaultLookAndFeel()).getNormalFont().withHeight(height);
		return Font(family, height, Font::plain);''')
save('FxSettingsDialog.cpp', s, bom)
print('ok')
