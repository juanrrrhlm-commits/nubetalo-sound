# Prueba de robustez: archivos .fac malformados en la carpeta de usuario; la app no debe caerse ni colgarse.
$root = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$exe = Join-Path $root 'app\bin\x64\NubetaloSound.exe'
$dir = Join-Path $env:APPDATA 'NubetaloSound\Presets'
$st  = Join-Path $env:APPDATA 'NubetaloSound\status.json'
New-Item -ItemType Directory -Force $dir | Out-Null
$rnd = New-Object Random 7
$cases = @{}
$cases['f_vacio'] = [byte[]]@()
$b = New-Object byte[] 200000; $rnd.NextBytes($b); $cases['f_binario'] = $b
$cases['f_linea_enorme'] = [Text.Encoding]::ASCII.GetBytes('Main0=' + ('9' * 3000000))
$cases['f_valores'] = [Text.Encoding]::ASCII.GetBytes("Main0=-99999999999`r`nMain1=1e999`r`nMain3=NaN`r`nBand1CF=0`r`nBand1Boost=99999`r`nBand2CF=-5`r`n")
$cases['f_nombre_ruta'] = [Text.Encoding]::ASCII.GetBytes("Name=..\..\..\Windows\evil`r`nMain0=5`r`n")
$cases['f_muchas_lineas'] = [Text.Encoding]::ASCII.GetBytes((1..300000 | ForEach-Object { "Band$($_)CF=100" }) -join "`r`n")
$cases['f_utf16'] = [Text.Encoding]::Unicode.GetBytes("Main0=5`r`nMain1=5`r`n")
foreach ($k in $cases.Keys) { [IO.File]::WriteAllBytes((Join-Path $dir "$k.fac"), $cases[$k]) }
Stop-Process -Name NubetaloSound -Force -ErrorAction SilentlyContinue; Start-Sleep 2
$p = Start-Process $exe -WorkingDirectory (Split-Path $exe) -PassThru
Start-Sleep 9
"proceso vivo tras arranque: $(-not $p.HasExited)"
Remove-Item $st -ErrorAction SilentlyContinue; & $exe --status | Out-Null; Start-Sleep 2
$s = Get-Content $st -Raw | ConvertFrom-Json
"estado leido: power=$($s.power) preajuste=$($s.selected_preset) | presets=$(($s.presets.app_defined | % { $_.name }) -join ',') | usuario=$(($s.presets.user_defined | % { $_.name }) -join ',')"
foreach ($n in $s.presets.user_defined) { & $exe "--preset=$($n.name)" | Out-Null; Start-Sleep -Milliseconds 400 }
"proceso vivo tras seleccionar todos: $(-not (Get-Process -Id $p.Id -ErrorAction SilentlyContinue).HasExited)"
foreach ($k in $cases.Keys) { Remove-Item (Join-Path $dir "$k.fac") -ErrorAction SilentlyContinue }
& $exe '--preset=Equilibrado' | Out-Null
