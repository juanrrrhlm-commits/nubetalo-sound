# Envía Ctrl+Alt+N (encender/apagar) con teclado real y comprueba el estado.
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$st = Join-Path $env:APPDATA 'NubetaloSound\status.json'
Add-Type @"
using System; using System.Runtime.InteropServices;
public class K { [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint fl, UIntPtr ex); }
"@
function Press($vks) { foreach ($v in $vks) { [K]::keybd_event($v,0,0,[UIntPtr]::Zero) }; Start-Sleep -Milliseconds 120; foreach ($v in [array]::Reverse($vks)) {}; for ($i=$vks.Count-1; $i -ge 0; $i--) { [K]::keybd_event($vks[$i],0,2,[UIntPtr]::Zero) } }
function Stat { Remove-Item $st -ErrorAction SilentlyContinue; & $exe --status | Out-Null; Start-Sleep 1.5; (Get-Content $st -Raw | ConvertFrom-Json) }
Stop-Process -Name NubetaloSound -Force -ErrorAction SilentlyContinue; Start-Sleep 2
Start-Process $exe -WorkingDirectory (Split-Path $exe); Start-Sleep 6
& $exe --power=1 | Out-Null; Start-Sleep 1
"antes: power=$((Stat).power)"
Press @(0x11,0x12,0x4E); Start-Sleep 1.5      # Ctrl+Alt+N
"tras Ctrl+Alt+N: power=$((Stat).power)"
Press @(0x11,0x12,0x4E); Start-Sleep 1.5
"tras segunda pulsación: power=$((Stat).power)"
