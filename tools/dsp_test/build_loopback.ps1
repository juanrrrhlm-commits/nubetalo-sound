$d=$PSScriptRoot; Push-Location $d
$vc='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
cmd /c "call `"$vc`" >nul && cl /nologo /EHsc /O2 /MT loopback.cpp ole32.lib uuid.lib /Fe:loopback.exe" 2>&1 | Select-Object -Last 3
Pop-Location
