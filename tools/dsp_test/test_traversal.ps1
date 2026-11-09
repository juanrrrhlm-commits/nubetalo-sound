# Un .fac importado con nombre "..\..\evil" no debe escribir fuera de la carpeta de presets.
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$exp = Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Nubetalo Sound\Presets\Export'
$out = Join-Path $env:APPDATA 'evilpreset.fac'
$log = Join-Path $env:TEMP 'nubetalo_presetio.log'
New-Item -ItemType Directory -Force $exp | Out-Null
Remove-Item $out, $log -ErrorAction SilentlyContinue
$lines = Get-Content (Join-Path $root 'app\bin\x64\Factsoft\1.fac')
$lines[2] = '..\..\evilpreset'
Set-Content -Path (Join-Path $exp 'evil.fac') -Value $lines
Stop-Process -Name NubetaloSound -Force -ErrorAction SilentlyContinue; Start-Sleep 2
$env:FXSOUND_PRESETIO = 'import'
$p = Start-Process $exe -WorkingDirectory (Split-Path $exe) -PassThru; $p.WaitForExit(30000) | Out-Null
Remove-Item Env:FXSOUND_PRESETIO
"import: " + (Get-Content $log -ErrorAction SilentlyContinue | Select-Object -Last 1)
"archivo escapado creado fuera de la carpeta: $(Test-Path $out)"
Remove-Item (Join-Path $exp 'evil.fac'), $out -ErrorAction SilentlyContinue

