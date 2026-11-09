"""Endurecimiento: nombres de preajuste seguros (sin rutas) y límite de tamaño al leer archivos .fac."""
G = 'C:/Users/juanr/Documents/fxsound-mejorado/app/fxsound/Source/GUI/'

def rw(p, fn):
    raw = open(G + p, 'rb').read()
    bom = raw.startswith(b'\xef\xbb\xbf')
    nl = '\r\n' if b'\r\n' in raw else '\n'
    t = fn(raw.decode('utf-8-sig').replace('\r\n', '\n'))
    open(G + p, 'wb').write((b'\xef\xbb\xbf' if bom else b'') + t.replace('\n', nl).encode('utf-8'))

def rep(t, a, b, count=1):
    assert t.count(a) >= 1, a[:70]
    return t.replace(a, b) if count == 0 else t.replace(a, b, count)

def model_h(t):
    return rep(t, '    bool isPresetNameValid(const String& preset_name);',
               '    bool isPresetNameValid(const String& preset_name);\n    // Nombre usable como archivo: sin rutas, sin caracteres reservados de Windows, 1-64 caracteres.\n    static bool isSafePresetName(const String& preset_name);')

def model_cpp(t):
    t = rep(t, 'bool FxModel::isPresetNameValid(const String& preset_name)\n{\n',
            '''bool FxModel::isSafePresetName(const String& n)
{
    if (n.isEmpty() || n.length() > 64 || n != n.trim() || n.startsWithChar('.'))
        return false;
    if (n.containsAnyOf("\\\\/:*?\\"<>|"))
        return false;
    for (auto c : n)
        if (c < 32)
            return false;

    static const StringArray reserved = { "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
                                          "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };
    return !reserved.contains(n.upToFirstOccurrenceOf(".", false, false).toUpperCase());
}

bool FxModel::isPresetNameValid(const String& preset_name)
{
    if (!isSafePresetName(preset_name))
        return false;

''')
    return t

def controller(t):
    big = 'if (path.getSize() > 65536) continue; // un preajuste real pesa pocos KB\n\t\t'
    a = '	for (auto path : app_preset_paths)\n	{\n'
    t = rep(t, a, a + '\t\t' + big.rstrip('\t'))
    a = '	for (auto path : user_preset_paths)\n	{\n'
    t = rep(t, a, a + '\t\t' + big.rstrip('\t'))
    a = '	for (auto preset_file : preset_files)\n	{\n'
    t = rep(t, a, a + '\t\tif (preset_file.getSize() > 65536) continue;\n')
    a = '	else\n	{\n		auto path = File::addTrailingSeparator(File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName()) + L"NubetaloSound\\\\Presets";\n		dfx_dsp_.savePreset(preset_name.toWideCharPointer()'
    t = rep(t, a, '	else if (!FxModel::isSafePresetName(preset_name))\n	{\n		return;\n	}\n' + a)
    a = '	if (preset.name == new_name) return;\n'
    t = rep(t, a, a + '	if (!FxModel::isSafePresetName(new_name)) return;\n')
    return t

rw('FxModel.h', model_h)
rw('FxModel.cpp', model_cpp)
rw('FxController.cpp', controller)
print('ok')
