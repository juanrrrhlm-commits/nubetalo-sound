# Projucer regenera FxSound_App.vcxproj sin las referencias a DfxDsp y audiopassthru: las vuelve a añadir.
import sys
p = sys.argv[1]
s = open(p, encoding='utf-8-sig').read()
if 'DfxDsp.vcxproj' not in s:
    nl = '\r\n' if '\r\n' in s else '\n'
    ref = (r'  <ItemGroup>' + '\n' + r'    <ProjectReference Include="..\..\dsp\DfxDsp.vcxproj" />' + '\n'
           + r'    <ProjectReference Include="..\..\audiopassthru\audiopassthru.vcxproj" />' + '\n' + r'  </ItemGroup>' + '\n').replace('\n', nl)
    i = s.rindex('</Project>')
    open(p, 'w', encoding='utf-8-sig', newline='').write(s[:i] + ref + s[i:])
    print('referencias añadidas')
