@echo off
REM just count every files
set /a targetLineN = 1024

set /a ftwNumLines = 0
set /a ftwNumLinesTest = 0
for /r %1 %%F in (*.h *.cpp) do (
  for /f %%N in ('find /v /c "" ^<"%%F"') do set /a ftwNumLines+=%%N
)
for /r %1 %%F in (*_test.cpp) do (
  for /f %%N in ('find /v /c "" ^<"%%F"') do set /a ftwNumLinesTest+=%%N
)

set /a TotalLineN = %ftwNumLines%
set /a TestLineN = %ftwNumLinesTest%
set /a FTWLineN = %ftwNumLines%-%ftwNumLinesTest%
set /a FTWPercent = %FTWLineN%*100/%TotalLineN%
set /a FTWPercentTarget = %FTWLineN%*100/%targetLineN%

del KeldenLineN*.txt
echo %date% %time% > "KeldenLineN%FTWLineN%.txt"
echo Total Code %TotalLineN% >> "KeldenLineN%FTWLineN%.txt"
echo Test Code %TestLineN% >> "KeldenLineN%FTWLineN%.txt"
echo FTW Code %FTWLineN% >> "KeldenLineN%FTWLineN%.txt"
echo. >> "KeldenLineN%FTWLineN%.txt"
echo %FTWPercent% %% of the Total code >> "KeldenLineN%FTWLineN%.txt"
echo %FTWPercentTarget% %% of target (%targetLineN%) >> "KeldenLineN%FTWLineN%.txt"