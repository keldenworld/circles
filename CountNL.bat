@echo off
REM just count every files
set /a targetNL = 8192

set /a ftwNumLines = 0
set /a ftwNumLinesTest = 0
for /r %1 %%F in (*.h *.cpp) do (
  for /f %%N in ('find /v /c "" ^<"%%F"') do set /a ftwNumLines+=%%N
)
for /r %1 %%F in (*_test.cpp) do (
  for /f %%N in ('find /v /c "" ^<"%%F"') do set /a ftwNumLinesTest+=%%N
)

set /a TotalNL = %ftwNumLines%
set /a TestNL = %ftwNumLinesTest%
set /a FTWNL = %ftwNumLines%-%ftwNumLinesTest%
set /a FTWPercent = %FTWNL%*100/%TotalNL%
set /a FTWPercentTarget = %FTWNL%*100/%targetNL%

del KeldenNL*.txt
echo %date% %time% > "KeldenNL%FTWNL%.txt"
echo Total Code %TotalNL% >> "KeldenNL%FTWNL%.txt"
echo Test Code %TestNL% >> "KeldenNL%FTWNL%.txt"
echo FTW Code %FTWNL% >> "KeldenNL%FTWNL%.txt"
echo. >> "KeldenNL%FTWNL%.txt"
echo %FTWPercent% %% of the Total code >> "KeldenNL%FTWNL%.txt"
echo %FTWPercentTarget% %% of target (%targetNL%) >> "KeldenNL%FTWNL%.txt"