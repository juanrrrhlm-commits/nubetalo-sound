$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe  = Join-Path $root 'app\bin\x64\NubetaloSound.exe'; $loop = Join-Path $PSScriptRoot 'loopback.exe'; $tp = Join-Path $PSScriptRoot 'tone_player.exe'; $log = Join-Path $PSScriptRoot 'tone_player2.log'
function Ap([string[]]$a) { & $exe @a | Out-Null }
function Lvl($n) { $o = (& $loop $n 0.6) -join ' '; if ($o -match 'L (-?[\d\.]+) dBFS') { [double]$Matches[1] } else { -999 } }
function Snap($t) { "  {0,-28} AirPods {1,6:N1} | Realtek {2,6:N1} | {3}" -f $t, (Lvl 'Auriculares (AirPods'), (Lvl 'Altavoces (Realtek'), (& $loop --default) }
Ap @('--power=1'); Start-Sleep 1; Ap @('--output=Auriculares (AirPods Pro de Juan)'); Start-Sleep 1
$proc = Start-Process $tp -ArgumentList '30','reabrir' -RedirectStandardOutput $log -NoNewWindow -PassThru; Start-Sleep 2
Snap 'power=1 (inicio)'
Ap @('--power=0'); Start-Sleep 2; Snap 'power=0'
Ap @('--power=1'); Start-Sleep 3; Snap 'power=1 (3 s despues)'
Start-Sleep 2; Snap 'power=1 (5 s despues)'
$proc.WaitForExit(30000) | Out-Null; "`nReproductor:"; Get-Content $log | Select-Object -Last 6
