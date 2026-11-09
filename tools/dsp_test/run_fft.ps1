$d = $PSScriptRoot; Push-Location $d
$vc = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
cmd /c "call `"$vc`" >nul && cl /nologo /EHsc /O2 /I ..\..\app\audiopassthru\include fft_test.cpp /Fe:fft_test.exe" 2>&1 | Select-Object -Last 3
& .\fft_test.exe
Pop-Location
