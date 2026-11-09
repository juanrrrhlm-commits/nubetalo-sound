# Genera el logo (SVG) y los iconos (.ico) de la nueva marca: anillo abierto + onda.
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$root   = Split-Path $PSScriptRoot -Parent
$images = Join-Path $root 'app\fxsound\Images'
$proj   = Join-Path $root 'app\fxsound\Project'

$ringHex = '#2F7BFF'; $waveHex = '#8DBFFF'; $inkHex = '#DCE6F5'

# ---------- Wordmark: texto -> contorno vectorial ----------
$fam = New-Object System.Drawing.FontFamily('Segoe UI Semibold')
$gp  = New-Object System.Drawing.Drawing2D.GraphicsPath
$sf  = New-Object System.Drawing.StringFormat
$gp.AddString('Nubetalo Sound', $fam, [int][System.Drawing.FontStyle]::Regular, 15.5, (New-Object System.Drawing.PointF(30, 3.2)), $sf)
$gp.Flatten($null, 0.03)
$bounds = $gp.GetBounds()
$sb = New-Object System.Text.StringBuilder
$pts = $gp.PathPoints; $types = $gp.PathTypes
for ($i = 0; $i -lt $pts.Length; $i++) {
    $t = $types[$i] -band 7
    $x = [math]::Round($pts[$i].X, 2).ToString([cultureinfo]::InvariantCulture)
    $y = [math]::Round($pts[$i].Y, 2).ToString([cultureinfo]::InvariantCulture)
    if ($t -eq 0) { [void]$sb.Append("M$x $y") } else { [void]$sb.Append("L$x $y") }
    if (($types[$i] -band 0x80) -ne 0) { [void]$sb.Append('Z') }
}
$textPath = $sb.ToString()

function Mark-Svg($ring, $wave) {
    "<path d='M16.35 5.05A8.2 8.2 0 1 1 7.65 5.05' fill='none' stroke='$ring' stroke-width='2.2' stroke-linecap='round'/>" +
    "<path d='M7.4 12.6C8.9 8.6 10.5 8.6 12 12.6S15.1 16.6 16.6 12.6' fill='none' stroke='$wave' stroke-width='1.7' stroke-linecap='round'/>"
}

$w = 30 + [math]::Ceiling($bounds.Width) + 4
$logo = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='-1 -1 $($w + 2) 26'>" + (Mark-Svg $ringHex $waveHex) +
        "<path d='$textPath' fill='$inkHex' fill-rule='evenodd'/></svg>"
$logoHover = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='-1 -1 $($w + 2) 26'>" + (Mark-Svg '#5AA0FF' '#CFE4FF') +
        "<path d='$textPath' fill='#FFFFFF' fill-rule='evenodd'/></svg>"
$markOnly = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24'>" + (Mark-Svg $ringHex $waveHex) + '</svg>'

[IO.File]::WriteAllText((Join-Path $images 'logo-white.svg'), $logo)
[IO.File]::WriteAllText((Join-Path $images 'logo-red.svg'), $logoHover)
[IO.File]::WriteAllText((Join-Path $images 'FxSound White Bars.svg'), $markOnly)
Write-Host "SVG listos (ancho del wordmark: $w)"

# ---------- Iconos ----------
function New-MarkBitmap([int]$size, [string]$ring, [string]$wave, [bool]$tile, [bool]$glow) {
    $ss = 4; $S = $size * $ss
    $bmp = New-Object System.Drawing.Bitmap($S, $S, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = 'AntiAlias'; $g.Clear([System.Drawing.Color]::Transparent)
    $u = $S / 24.0
    if ($tile) {
        $rad = 5.4 * $u
        $p = New-Object System.Drawing.Drawing2D.GraphicsPath
        $d = $rad * 2
        $p.AddArc(0, 0, $d, $d, 180, 90); $p.AddArc($S - $d, 0, $d, $d, 270, 90)
        $p.AddArc($S - $d, $S - $d, $d, $d, 0, 90); $p.AddArc(0, $S - $d, $d, $d, 90, 90); $p.CloseFigure()
        $g.FillPath((New-Object System.Drawing.SolidBrush([System.Drawing.ColorTranslator]::FromHtml('#07090D'))), $p)
        $g.DrawPath((New-Object System.Drawing.Pen([System.Drawing.ColorTranslator]::FromHtml('#1A2A45'), [single](0.7 * $u))), $p)
    }
    $ringPath = New-Object System.Drawing.Drawing2D.GraphicsPath
    $ringPath.AddArc((12 - 8.2) * $u, (12 - 8.2) * $u, 16.4 * $u, 16.4 * $u, 302, 296)   # anillo abierto arriba
    $wavePath = New-Object System.Drawing.Drawing2D.GraphicsPath
    $wavePath.AddBezier([single](7.4*$u), [single](12.6*$u), [single](8.9*$u), [single](8.6*$u), [single](10.5*$u), [single](8.6*$u), [single](12*$u), [single](12.6*$u))
    $wavePath.AddBezier([single](12*$u), [single](12.6*$u), [single](13.5*$u), [single](16.6*$u), [single](15.1*$u), [single](16.6*$u), [single](16.6*$u), [single](12.6*$u))
    $ringC = [System.Drawing.ColorTranslator]::FromHtml($ring); $waveC = [System.Drawing.ColorTranslator]::FromHtml($wave)
    if ($glow) {
        $gl = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(70, $ringC), [single](5.2 * $u))
        $gl.StartCap = 'Round'; $gl.EndCap = 'Round'
        $g.DrawPath($gl, $ringPath)
    }
    $pr = New-Object System.Drawing.Pen($ringC, [single](2.2 * $u)); $pr.StartCap = 'Round'; $pr.EndCap = 'Round'
    $pw = New-Object System.Drawing.Pen($waveC, [single](1.7 * $u)); $pw.StartCap = 'Round'; $pw.EndCap = 'Round'
    $g.DrawPath($pr, $ringPath); $g.DrawPath($pw, $wavePath)
    $g.Dispose()
    $out = New-Object System.Drawing.Bitmap($size, $size, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g2 = [System.Drawing.Graphics]::FromImage($out)
    $g2.InterpolationMode = 'HighQualityBicubic'; $g2.PixelOffsetMode = 'HighQuality'; $g2.SmoothingMode = 'HighQuality'
    $g2.DrawImage($bmp, 0, 0, $size, $size); $g2.Dispose(); $bmp.Dispose()
    return $out
}

function Write-Ico([string]$path, [string]$ring, [string]$wave, [bool]$tile) {
    $sizes = 16, 20, 24, 32, 40, 48, 64, 128, 256
    $pngs = @()
    foreach ($s in $sizes) {
        $b = New-MarkBitmap $s $ring $wave $tile ($s -ge 48)
        $ms = New-Object System.IO.MemoryStream
        $b.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png); $pngs += , $ms.ToArray(); $b.Dispose()
    }
    $fs = [IO.File]::Create($path); $bw = New-Object IO.BinaryWriter($fs)
    $bw.Write([uint16]0); $bw.Write([uint16]1); $bw.Write([uint16]$sizes.Count)
    $offset = 6 + 16 * $sizes.Count
    for ($i = 0; $i -lt $sizes.Count; $i++) {
        $s = $sizes[$i]; $dim = if ($s -ge 256) { 0 } else { $s }
        $bw.Write([byte]$dim); $bw.Write([byte]$dim); $bw.Write([byte]0); $bw.Write([byte]0)
        $bw.Write([uint16]1); $bw.Write([uint16]32); $bw.Write([uint32]$pngs[$i].Length); $bw.Write([uint32]$offset)
        $offset += $pngs[$i].Length
    }
    foreach ($p in $pngs) { $bw.Write($p) }
    $bw.Close(); $fs.Close()
}

Write-Ico (Join-Path $proj 'icon.ico')       '#2F7BFF' '#8DBFFF' $true     # app / ventana
Write-Ico (Join-Path $proj 'white_logo.ico') '#FFFFFF' '#C9D6EA' $true    # bandeja, claro
Write-Ico (Join-Path $proj 'red_logo.ico')   '#2F7BFF' '#8DBFFF' $true    # bandeja, procesando (ahora cobalto)
Write-Ico (Join-Path $proj 'blue_logo.ico')  '#2F7BFF' '#8DBFFF' $true
Write-Ico (Join-Path $proj 'gray_logo.ico')  '#7C8798' '#A9B3C2' $true    # bandeja, apagado
# Vista previa para revisar el diseño
$prev = New-MarkBitmap 400 '#2F7BFF' '#8DBFFF' $true $true
$prev.Save((Join-Path $root 'tools\icono_preview.png'), [System.Drawing.Imaging.ImageFormat]::Png)
Write-Host 'Iconos listos'
