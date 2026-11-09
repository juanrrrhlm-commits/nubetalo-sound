# Genera el instalador: release limpio -> imágenes -> zip de fuentes -> Inno Setup.
# Uso: powershell -File tools\make_installer.ps1
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$iscc = Join-Path $env:LOCALAPPDATA 'Programs\Inno Setup 6\ISCC.exe'
$ver  = '1.0.0'

Copy-Item (Join-Path $root 'app\LICENSE') (Join-Path $root 'LICENSE.txt') -Force
New-Item -ItemType Directory -Force (Join-Path $root 'installer\output'), (Join-Path $root 'installer\assets') | Out-Null

python -I (Join-Path $PSScriptRoot 'make_installer_assets.py')
if ($LASTEXITCODE) { throw 'imágenes' }
python -I (Join-Path $PSScriptRoot 'make_source_zip.py') $ver
if ($LASTEXITCODE) { throw 'zip de fuentes' }

& $iscc (Join-Path $root 'installer\NubetaloSound.iss')
if ($LASTEXITCODE) { throw 'Inno Setup' }
Get-FileHash (Join-Path $root "installer\output\NubetaloSound-Setup-$ver.exe") -Algorithm SHA256 | Format-List Hash, Path
