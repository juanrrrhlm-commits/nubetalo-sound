param([int]$w=520,[int]$h=110,[string]$out="tools\shot.png",[int]$delay=450)
Add-Type -AssemblyName System.Drawing, System.Windows.Forms
Start-Sleep -Milliseconds $delay
$b = New-Object System.Drawing.Bitmap $w,$h
$g = [System.Drawing.Graphics]::FromImage($b); $g.CopyFromScreen(0,0,0,0,$b.Size)
$b.Save((Join-Path (Get-Location) $out)); $g.Dispose(); $b.Dispose()
