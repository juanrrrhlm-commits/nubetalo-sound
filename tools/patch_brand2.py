import sys
sys.path.insert(0, 'tools')
src = open('tools/patch_brand.py', encoding='utf-8').read().split("edit('Utils/Settings/Settings.h'")[0]
exec(src)
edit('GUI/FxMainWindow.cpp', [], regex=[
    (r'(?s)\tauto downloadClicked = \[\]\(\) \{.*?\n\t\};\n\n', ''),
    (r'(?s)\tauto checkForUpdatesClicked = \[\]\(\) \{.*?\n\t\};\n\n', ''),
])
edit('GUI/FxController.cpp', [('survey_tip_ = !settings_.getBool("survey_displayed");', 'survey_tip_ = false; // sin encuestas externas'),
    ('#include "FxSystemTrayView.h"', '#include "FxSystemTrayView.h"\n#include "FxSupport.h"')])
edit('GUI/FxView.cpp', [], regex=[(r'(#include "FxView.h"\n)', r'\1#include "FxSupport.h"\n')])
print('ok2')
