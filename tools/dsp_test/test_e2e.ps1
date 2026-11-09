# Prueba de punta a punta: reproduce audio, cambia ajustes por línea de comandos y mide lo que sale por los AirPods.
$ErrorActionPreference = 'Continue'
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe  = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$loop = Join-Path $PSScriptRoot 'loopback.exe'
$dev  = 'Auriculares (AirPods'
$status = Join-Path $env:APPDATA 'FxSound\status.json'

function Ap([string[]]$a) { & $exe @a | Out-Null; Start-Sleep -Milliseconds 900 }
function Status { Remove-Item $status -ErrorAction SilentlyContinue; & $exe --status | Out-Null; Start-Sleep -Milliseconds 1200; Get-Content $status -Raw | ConvertFrom-Json }
function Meas([double[]]$f) { $o = & $loop $dev 1.5 @f; ($o -join ' ') }
function Num($line, $label) { if ($line -match "$label\s+(-?[\d\.]+)") { [double]$Matches[1] } else { [double]::NaN } }

$s0 = Status
Write-Host ("Estado inicial: power={0} salida={1} preset={2}" -f $s0.power, $s0.selected_output, $s0.selected_preset)
$bands0 = $s0.equalizer.bands | ForEach-Object { "{0}:{1}" -f $_.index, $_.gain }
$eff0 = $s0.effects

if (-not $s0.power) { Ap @('--power=1') }
Ap @('--output=Auriculares (AirPods Pro de Juan)')

function ResetFlat {
    Ap @('--master_gain=0', '--balance=0', '--volume_leveling=0')
    Ap @('--set_effect=clarity:0,ambience:0,surround:0,dynamicboost:0,bass:0')
    Ap @(('--set_band_gain=' + ((0..9 | ForEach-Object { "$($_):0" }) -join ',')))
}

# ---------- Parte A: tonos 100 Hz / 1 kHz / 8 kHz ----------
$tonos = New-Object System.Media.SoundPlayer (Join-Path $PSScriptRoot 'tonos.wav')
$tonos.PlayLooping(); Start-Sleep 2
ResetFlat
$F = 100, 1000, 8000
$flat = Meas $F
Write-Host "`n[A] Referencia plana: $flat"
$b100 = Num $flat '100 Hz'; $b1k = Num $flat '1000 Hz'; $b8k = Num $flat '8000 Hz'
$flatL = Num $flat 'L'

function Row($titulo, $line) {
    $d100 = (Num $line '100 Hz') - $b100; $d1k = (Num $line '1000 Hz') - $b1k; $d8k = (Num $line '8000 Hz') - $b8k
    Write-Host ("  {0,-34} 100 Hz {1,7:+0.0;-0.0;0.0} | 1 kHz {2,7:+0.0;-0.0;0.0} | 8 kHz {3,7:+0.0;-0.0;0.0} dB" -f $titulo, $d100, $d1k, $d8k)
}

Write-Host "`n[A1] Ecualizador (cambio respecto a plano):"
Ap @('--set_band_freq=5:1000,8:8000')
foreach ($t in @(@('banda 1 (101 Hz) +12', '1:12'), @('banda 1 (101 Hz) -12', '1:-12'), @('banda 5 (1 kHz) +12', '5:12'), @('banda 5 (1 kHz) -12', '5:-12'), @('banda 8 (8 kHz) +12', '8:12'), @('banda 8 (8 kHz) -12', '8:-12'))) {
    Ap @('--set_band_gain=' + $t[1]); Row $t[0] (Meas $F)
    Ap @(('--set_band_gain=' + ((0..9 | ForEach-Object { "$($_):0" }) -join ',')))
}

Write-Host "`n[A2] Efectos 0 -> 10:"
foreach ($e in 'bass', 'clarity', 'ambience', 'surround', 'dynamicboost') {
    Ap @("--set_effect=${e}:10"); Row "efecto $e = 10" (Meas $F); Ap @("--set_effect=${e}:0")
}

Write-Host "`n[A3] Ganancia maestra (tonos bajos, sin limitador):"
foreach ($g in -20, -10, 10, 20) { Ap @("--master_gain=$g"); Row "master $g dB" (Meas $F) }
Ap @('--master_gain=0')

Write-Host "`n[A4] Balance (L / R respecto a plano, dB):"
foreach ($b in -20, 20) { Ap @("--balance=$b"); $m = Meas $F; Write-Host ("  balance {0,4}: {1}" -f $b, $m) }
Ap @('--balance=0')

Write-Host "`n[A5] Apagado y encendido (bypass):"
Ap @('--power=0'); Write-Host ("  power=0: " + (Meas $F))
Ap @('--power=1'); Start-Sleep 1; Write-Host ("  power=1: " + (Meas $F))
$tonos.Stop()

# ---------- Parte B: audio tipo musica (-20 dBFS RMS) ----------
$mus = New-Object System.Media.SoundPlayer (Join-Path $PSScriptRoot 'musica.wav')
$mus.PlayLooping(); Start-Sleep 2
ResetFlat
$ref = Num (Meas @()) 'L'
Write-Host ("`n[B] Musica simulada, referencia plana: L {0:N1} dBFS" -f $ref)
foreach ($g in -10, 0, 6, 12, 20) { Ap @("--master_gain=$g"); $v = Num (Meas @()) 'L'; Write-Host ("  master {0,4} dB -> {1,7:+0.0;-0.0;0.0} dB respecto a 0 dB" -f $g, ($v - $ref)) }
Write-Host "  con Refuerzo dinamico = 10:"
Ap @('--set_effect=dynamicboost:10'); foreach ($g in 0, 12) { Ap @("--master_gain=$g"); $v = Num (Meas @()) 'L'; Write-Host ("  master {0,4} dB -> {1,7:+0.0;-0.0;0.0} dB" -f $g, ($v - $ref)) }
$mus.Stop()

# ---------- Restaurar ----------
Ap @('--master_gain=0', '--balance=0')
Ap @(('--set_band_gain=' + ($bands0 -join ',')))
Ap @(('--set_effect=clarity:{0},ambience:{1},surround:{2},dynamicboost:{3},bass:{4}' -f $eff0.clarity, $eff0.ambience, $eff0.surround, $eff0.dynamicboost, $eff0.bass))
Write-Host "`nAjustes restaurados."
