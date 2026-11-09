# Prueba del cambio de salida y del apagado/encendido con audio sonando.
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe  = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$loop = Join-Path $PSScriptRoot 'loopback.exe'
function Ap([string[]]$a) { & $exe @a | Out-Null; Start-Sleep -Milliseconds 800 }
function Lvl($name) { $o = (& $loop $name 1.0) -join ' '; if ($o -match 'L (-?[\d\.]+) dBFS') { [double]$Matches[1] } else { -999 } }
function Report($t) { $a = Lvl 'Auriculares (AirPods'; $r = Lvl 'Altavoces (Realtek'; $v = Lvl 'FxSound Speakers'; $d = (& $loop --default); Write-Host ("  {0,-40} AirPods {1,6:N1} | Altavoces {2,6:N1} | virtual {3,6:N1} | {4}" -f $t, $a, $r, $v, $d) }

$AIR = '--output=Auriculares (AirPods Pro de Juan)'; $SPK = '--output=Altavoces (Realtek(R) Audio)'
Ap @('--power=1'); Ap @($AIR); Ap @('--master_gain=0')
$p = New-Object System.Media.SoundPlayer (Join-Path $PSScriptRoot 'musica.wav'); $p.PlayLooping(); Start-Sleep 2

Write-Host "[C1] Cambio de salida con audio sonando:"
Report 'inicio (AirPods)'
foreach ($i in 1..3) { Ap @($SPK); Start-Sleep 1; Report "-> altavoces (vuelta $i)"; Ap @($AIR); Start-Sleep 1; Report "-> AirPods (vuelta $i)" }

Write-Host "`n[C2] Cambios rapidos (sin esperar) y luego medir:"
foreach ($i in 1..4) { & $exe $SPK | Out-Null; & $exe $AIR | Out-Null }
Start-Sleep 2; Report 'tras 4 ida y vuelta rapidas (debe ser AirPods)'

Write-Host "`n[C3] Apagar y encender con el mismo reproductor:"
Ap @('--power=0'); Start-Sleep 1; Report 'power=0'
Ap @('--power=1'); Start-Sleep 3; Report 'power=1 (3 s despues)'
$p.Stop()
$p2 = New-Object System.Media.SoundPlayer (Join-Path $PSScriptRoot 'musica.wav'); $p2.PlayLooping(); Start-Sleep 2
Report 'power=1 con reproductor nuevo'
$p2.Stop()
Write-Host "`nListo."
