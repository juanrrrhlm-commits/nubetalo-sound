"""Zip del código fuente que se entrega con el instalador (AGPL v3, sección 6)."""
import os, sys, zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
VER = sys.argv[1] if len(sys.argv) > 1 else '1.0.1'
OUT = os.path.join(ROOT, 'installer', 'output', f'Nubetalo-Sound-{VER}-source.zip')

TOP_FILES = ['README.md', 'LICENSE.txt', 'NOTICE.txt', 'PRIVACY.md', 'SECURITY.md']
TREES = ['app/audiopassthru', 'app/dsp', 'app/fxsound', 'app/fxdiag', 'app/lib', 'app/docs', 'app/Resources/Strings',
         'installer/NubetaloSound.iss', 'installer/assets']
TOOLS = ['build.sh', 'build_release.sh', 'resave.sh', 'patch_refs.py', 'make_brand.ps1', 'make_presets.py',
         'make_installer.ps1', 'make_installer_assets.py', 'make_source_zip.py', 'IBMPlexSans-OFL.txt']
SKIP_DIRS = {'x64', 'x86', 'arm64', 'Debug', 'Release', '.vs', 'JuceLibraryCode', 'Builds', 'ProjectARM', 'Project'}
SKIP_EXT = {'.obj', '.pdb', '.ilk', '.exe', '.dll', '.lib', '.log', '.tlog', '.idb', '.ipdb', '.iobj', '.vcxproj', '.sln', '.filters', '.user', '.sys', '.cat'}
KEEP_IN_PROJECT = {'.ico', '.rc'}   # Project\ lo regenera Projucer, salvo iconos y recursos propios


def keep(rel, name):
    ext = os.path.splitext(name)[1].lower()
    parts = rel.replace('\\', '/').split('/')
    if 'Project' in parts:
        return ext in KEEP_IN_PROJECT
    return ext not in SKIP_EXT


count = 0
with zipfile.ZipFile(OUT, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
    for f in TOP_FILES:
        z.write(os.path.join(ROOT, f), f); count += 1
    for t in TREES:
        p = os.path.join(ROOT, t)
        if os.path.isfile(p):
            z.write(p, t.replace('\\', '/')); count += 1
            continue
        for d, dirs, files in os.walk(p):
            rel_dir = os.path.relpath(d, ROOT)
            in_project = 'Project' in rel_dir.replace('\\', '/').split('/')
            dirs[:] = [x for x in dirs if x not in SKIP_DIRS or (x == 'Project')]
            if in_project and rel_dir.replace('\\', '/').split('/')[-1] != 'Project':
                dirs[:] = []
                continue
            for n in files:
                if keep(rel_dir, n):
                    z.write(os.path.join(d, n), os.path.join(rel_dir, n).replace('\\', '/')); count += 1
    for n in TOOLS:
        p = os.path.join(ROOT, 'tools', n)
        if os.path.exists(p):
            z.write(p, 'tools/' + n); count += 1
print(f'{count} archivos -> {OUT} ({os.path.getsize(OUT)/1e6:.1f} MB)')
