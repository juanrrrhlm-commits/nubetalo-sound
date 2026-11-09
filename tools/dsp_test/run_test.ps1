$d = $PSScriptRoot
Push-Location $d
$vc = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
cmd /c "call `"$vc`" >nul && cl /nologo /EHsc /O2 /MT /I ..\..\app\dsp\include dsp_test.cpp ..\..\app\lib\Release-x64\DfxDsp.lib ..\..\app\lib\Release-x64\audiopassthru.lib ole32.lib uuid.lib advapi32.lib user32.lib shlwapi.lib shell32.lib /Fe:dsp_test.exe" 2>&1 | Select-Object -Last 3
& .\dsp_test.exe
Pop-Location
