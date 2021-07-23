@echo off
set back=%cd%
for /d %%i in (%cd%\*) do (
cd "%%i"
echo current directory:
cd
pio run
pause
)
cd %back%