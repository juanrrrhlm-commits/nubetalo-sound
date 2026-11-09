param([string]$Mode = '')
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe  = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$loop = Join-Path $PSScriptRoot 'loopback.exe'
$tp   = Join-Path $PSScriptRoot 'tone_player.exe'
$log  = Join-Path $PSScriptRoot 'tone_player.log'
function Ap([string[]]$a) { & $exe @a | Out-Null }
function Lvl($name) { $o = (& $loop $name 0.6) -join ' '; if ($o -match 'L (-?[\d\.]+) dBFS') { [double]$Matches[1] } else { -999 } }
function Snap($t0, $tag) { $a = Lvl 'Auriculares (AirPods'; $r = Lvl 'Altavoces (Realtek'; "  t={0,5:N1}s {1,-26} AirPods {2,6:N1} | Realtek {3,6:N1}" -f ((Get-Date) - $t0).TotalSeconds, $tag, $a, $r }
$AIR = '--output=Auriculares (AirPods Pro de Juan)'; $SPK = '--output=Altavoces (Realtek(R) Audio)'
Ap @('--power=1'); Start-Sleep 1; Ap @($AIR); Start-Sleep 1
$args2 = @('40'); if ($Mode -eq 'reabrir') { $args2 += 'reabrir' }
$proc = Start-Process $tp -ArgumentList $args2 -RedirectStandardOutput $log -NoNewWindow -PassThru
Start-Sleep 2
$t0 = Get-Date
Snap $t0 'antes (AirPods)'
Ap @($SPK); Write-Host "  -- cambio a altavoces --"
1..4 | ForEach-Object { Snap $t0 'altavoces' ; Start-Sleep -Milliseconds 700 }
Ap @($AIR); Write-Host "  -- cambio a AirPods --"
1..4 | ForEach-Object { Snap $t0 'AirPods' ; Start-Sleep -Milliseconds 700 }
Ap @($SPK); Start-Sleep 1; Ap @($AIR); Write-Host "  -- ida y vuelta rapida --"
1..3 | ForEach-Object { Snap $t0 'final AirPods' ; Start-Sleep -Milliseconds 700 }
$proc.WaitForExit(30000) | Out-Null
Write-Host "`nRegistro del reproductor:"; Get-Content $log | Select-Object -Last 14
