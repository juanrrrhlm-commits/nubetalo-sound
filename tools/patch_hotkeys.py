G = 'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/Source/'

def load(p):
    b = open(G + p, 'rb').read()
    return b.decode('utf-8-sig').replace('\r\n', '\n'), b.startswith(b'\xef\xbb\xbf'), ('\r\n' if b'\r\n' in b else '\n')

def save(p, t, bom, nl):
    open(G + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.replace('\n', nl).encode('utf-8'))

c, bom, nl = load('GUI/FxController.cpp')
a = "	if ((mod == (MOD_CONTROL | MOD_ALT) || mod == (MOD_CONTROL | MOD_SHIFT)) && ((vk >= 0x30 && vk <= 0x39) || (vk >= 'A' && vk <= 'Z')))"
assert a in c
c = c.replace(a, "	const bool valid_mod = (mod & MOD_CONTROL) != 0 && (mod & (MOD_ALT | MOD_SHIFT)) != 0;\n	const bool valid_key = (vk >= 0x30 && vk <= 0x39) || (vk >= 'A' && vk <= 'Z') || (vk >= VK_F1 && vk <= VK_F12) || (vk >= VK_LEFT && vk <= VK_DOWN);\n	if (valid_mod && valid_key)")
a = "bool FxController::isValidHotkey(int mod, int vk)"
assert a in c
c = c.replace(a, '''// Atajos de fábrica: Ctrl + Alt + N (encender/apagar), V (ventana), P / Mayús + P (preajuste), D (dispositivo).
void FxController::resetHotkeys()
{
	const struct { const char* cmd; int mod; int vk; } defaults[] = {
		{ HK_CMD_ON_OFF, MOD_CONTROL | MOD_ALT, 'N' },
		{ HK_CMD_OPEN_CLOSE, MOD_CONTROL | MOD_ALT, 'V' },
		{ HK_CMD_NEXT_PRESET, MOD_CONTROL | MOD_ALT, 'P' },
		{ HK_CMD_PREVIOUS_PRESET, MOD_CONTROL | MOD_ALT | MOD_SHIFT, 'P' },
		{ HK_CMD_NEXT_OUTPUT, MOD_CONTROL | MOD_ALT, 'D' } };

	for (auto& d : defaults)
		setHotkey(d.cmd, 0, 0); // libera primero: evita choques entre los viejos y los nuevos

	for (auto& d : defaults)
		setHotkey(d.cmd, d.mod, d.vk);
}

''' + a, 1)
save('GUI/FxController.cpp', c, bom, nl)

h, bom, nl = load('GUI/FxController.h')
a = '	bool setHotkey(const String& command, int new_mod, int new_vk);'
assert a in h
h = h.replace(a, a + '\n	void resetHotkeys();')
save('GUI/FxController.h', h, bom, nl)

s, bom, nl = load('Utils/Settings/Settings.cpp')
for old, new in (('"cmd_on_off" val="393297"', '"cmd_on_off" val="196686"'),
                 ('"cmd_open_close" val="393285"', '"cmd_open_close" val="196694"'),
                 ('"cmd_next_preset" val="393281"', '"cmd_next_preset" val="196688"'),
                 ('"cmd_previous_preset" val="393306"', '"cmd_previous_preset" val="458832"'),
                 ('"cmd_change_output" val="393303"', '"cmd_change_output" val="196676"')):
    assert old in s, old
    s = s.replace(old, new)
save('Utils/Settings/Settings.cpp', s, bom, nl)
print('ok')
