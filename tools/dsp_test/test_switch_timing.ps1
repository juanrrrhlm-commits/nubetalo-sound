$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe  = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$loop = Join-Path $PSScriptRoot 'loopback.exe'
function Ap([string[]]$a) { & $exe @a | Out-Null }
function Lvl($name) { $o = (& $loop $name 0.8) -join ' '; if ($o -match 'L (-?[\d\.]+) dBFS') { [double]$Matches[1] } else { -999 } }
function Snap($t0, $tag) { $a = Lvl 'Auriculares (AirPods'; $r = Lvl 'Altavoces (Realtek'; $v = Lvl '2- FxSound'; "  t={0,4:N1}s {1,-22} AirPods {2,6:N1} | Realtek {3,6:N1} | virtual {4,6:N1}" -f ((Get-Date) - $t0).TotalSeconds, $tag, $a, $r, $v }
$AIR = '--output=Auriculares (AirPods Pro de Juan)'; $SPK = '--output=Altavoces (Realtek(R) Audio)'
Ap @('--power=1'); Start-Sleep 1; Ap @($AIR); Start-Sleep 1
$p = New-Object System.Media.SoundPlayer (Join-Path $PSScriptRoot 'musica.wav'); $p.PlayLooping(); Start-Sleep 2
Write-Host "[T1] Un cambio AirPods -> Altavoces, midiendo cada ~2.5 s:"
Snap (Get-Date) 'antes (AirPods)'
$t0 = Get-Date; Ap @($SPK)
1..8 | ForEach-Object { Snap $t0 'tras cambiar a altavoces'; Start-Sleep -Milliseconds 1200 }
Write-Host "`n[T2] Un cambio Altavoces -> AirPods:"
$t0 = Get-Date; Ap @($AIR)
1..8 | ForEach-Object { Snap $t0 'tras cambiar a AirPods'; Start-Sleep -Milliseconds 1200 }
$p.Stop()
Write-Host "`n[T3] Reproductor NUEVO despues de un cambio (altavoces):"
Ap @($SPK); Start-Sleep 2
$p2 = New-Object System.Media.SoundPlayer (Join-Path $PSScriptRoot 'musica.wav'); $p2.PlayLooping(); Start-Sleep 1.5
Snap (Get-Date) 'reproductor nuevo'
$p2.Stop(); Ap @($AIR); Start-Sleep 2
Write-Host "`nListo (salida final: AirPods)."
