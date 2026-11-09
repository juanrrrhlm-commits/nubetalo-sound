# Ciclo completo: crear preset de usuario -> exportar -> borrar -> importar -> comprobar.
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe  = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$wd   = Split-Path $exe
$user = Join-Path $env:APPDATA 'FxSound\Presets'
$exp  = Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Nubetalo Sound\Presets\Export'
$log  = Join-Path $root 'tools\presetio.log'
$st   = Join-Path $env:APPDATA 'FxSound\status.json'
function Ap([string[]]$a) { & $exe @a | Out-Null; Start-Sleep -Milliseconds 900 }
function Stat { Remove-Item $st -ErrorAction SilentlyContinue; & $exe --status | Out-Null; Start-Sleep 1.3; Get-Content $st -Raw | ConvertFrom-Json }
function Stop-App { Stop-Process -Name NubetaloSound -Force -ErrorAction SilentlyContinue; Start-Sleep 2 }
function Run-Hook($mode) { $env:FXSOUND_PRESETIO = $mode; $p = Start-Process $exe -WorkingDirectory $wd -PassThru; $p.WaitForExit(30000) | Out-Null; Remove-Item Env:FXSOUND_PRESETIO }

Remove-Item $log -ErrorAction SilentlyContinue
Remove-Item "$exp\PruebaIO.fac" -ErrorAction SilentlyContinue; Remove-Item "$user\PruebaIO.fac" -ErrorAction SilentlyContinue

Stop-App; Start-Process $exe -WorkingDirectory $wd; Start-Sleep 6
Ap @('--power=1'); Ap @('--preset=Equilibrado'); Ap @('--set_effect=bass:6,clarity:5')
Ap @('--save_preset=PruebaIO'); Start-Sleep 1
$s = Stat
"1) Crear: seleccionado=$($s.selected_preset) | en 'usuario'=$(($s.presets.user_defined | ForEach-Object { $_.name }) -join ',') | archivo=$(Test-Path "$user\PruebaIO.fac") | bass=$([math]::Round($s.effects.bass,1)) clarity=$([math]::Round($s.effects.clarity,1))"

Stop-App; Run-Hook 'export'
"2) Exportar: " + (Get-Content $log -ErrorAction SilentlyContinue | Select-Object -Last 1) + " | archivo exportado=$(Test-Path "$exp\PruebaIO.fac")"

Start-Process $exe -WorkingDirectory $wd; Start-Sleep 6
Ap @('--preset=PruebaIO'); Ap @('--delete_preset'); Start-Sleep 1
$s = Stat
"3) Borrar: en 'usuario'=$(($s.presets.user_defined | ForEach-Object { $_.name }) -join ',') | archivo=$(Test-Path "$user\PruebaIO.fac")"

Stop-App; Run-Hook 'import'
"4) Importar: " + (Get-Content $log -ErrorAction SilentlyContinue | Select-Object -Last 1)

Start-Process $exe -WorkingDirectory $wd; Start-Sleep 6
Ap @('--preset=PruebaIO'); Start-Sleep 1
$s = Stat
"5) Tras importar: seleccionado=$($s.selected_preset) | en 'usuario'=$(($s.presets.user_defined | ForEach-Object { $_.name }) -join ',') | bass=$([math]::Round($s.effects.bass,1)) clarity=$([math]::Round($s.effects.clarity,1))"

# limpieza
Ap @('--preset=PruebaIO'); Ap @('--delete_preset'); Ap @('--preset=Equilibrado')
Remove-Item "$exp\PruebaIO.fac" -ErrorAction SilentlyContinue
$s = Stat; "6) Limpieza: usuario=$(($s.presets.user_defined | ForEach-Object { $_.name }) -join ',') | seleccionado=$($s.selected_preset)"
