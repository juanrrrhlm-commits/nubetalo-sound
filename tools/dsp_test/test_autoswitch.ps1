# Prueba del cambio automático al conectar auriculares (Bluetooth): fuerza altavoces, apaga y enciende Bluetooth y observa la salida elegida.
# Requiere unos auriculares Bluetooth ya emparejados y conectados. Uso: powershell -File test_autoswitch.ps1 -Headphones "Auriculares (AirPods Pro de Juan)" -Speakers "Altavoces (Realtek(R) Audio)"
param([string]$Headphones = 'Auriculares (AirPods Pro de Juan)', [string]$Speakers = 'Altavoces (Realtek(R) Audio)')
$exe = Join-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) 'app\bin\x64\NubetaloSound.exe'
function Cfg { $t = Get-Content "$env:APPDATA\NubetaloSound\NubetaloSound.settings" -Raw; if ($t -match 'name="output_device_name"\s+val="([^"]*)"') { $Matches[1] } else { '?' } }
function Stat { try { (Get-Content "$env:APPDATA\NubetaloSound\status.json" -Raw | ConvertFrom-Json).selected_output } catch { '?' } }
function Air { @(Get-PnpDevice -Class AudioEndpoint -Status OK -ErrorAction SilentlyContinue | Where-Object { $_.FriendlyName -eq $Headphones }).Count }

Add-Type -AssemblyName System.Runtime.WindowsRuntime
$null = [Windows.Devices.Radios.Radio, Windows.System.Devices, ContentType=WindowsRuntime]
$asTask = ([System.WindowsRuntimeSystemExtensions].GetMethods() | Where-Object { $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]
function Await($op, [type]$t) { $m = $asTask.MakeGenericMethod($t); $x = $m.Invoke($null, @($op)); $x.Wait(-1) | Out-Null; $x.Result }
$radioList = [System.Collections.Generic.IReadOnlyList[Windows.Devices.Radios.Radio]]
$access = [Windows.Devices.Radios.RadioAccessStatus]
$bt = (Await ([Windows.Devices.Radios.Radio]::GetRadiosAsync()) $radioList) | Where-Object { $_.Kind -eq 'Bluetooth' }

Get-Process NubetaloSound -ErrorAction SilentlyContinue | Stop-Process -Force
$p = Start-Process $exe -WorkingDirectory (Split-Path $exe) -PassThru
Start-Sleep 12
Start-Process $exe -ArgumentList ('--output="{0}"' -f $Speakers) -WorkingDirectory (Split-Path $exe) -Wait
Start-Sleep 6
"1) tras forzar altavoces: ajustes='$(Cfg)' estado='$(Stat)'"
$null = Await ($bt.SetStateAsync('Off')) $access
Start-Sleep 10
"2) Bluetooth apagado: auriculares presentes=$(Air) ajustes='$(Cfg)'"
$null = Await ($bt.SetStateAsync('On')) $access
$ok = $false
for ($i = 1; $i -le 15; $i++) {
  Start-Sleep 3
  $c = Cfg; $s = Stat
  "3) t=$($i*3)s auriculares presentes=$(Air) ajustes='$c' estado='$s'"
  if ($c -eq $Headphones) { $ok = $true; break }
}
if ($ok) { 'RESULTADO: cambio automatico a los auriculares OK' } else { 'RESULTADO: NO cambio' }
