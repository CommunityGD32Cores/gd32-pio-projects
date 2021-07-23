@echo off
set back=%cd%
for /d %%i in (%cd%\*) do (
cd "%%i"
echo current directory:
cd
if exist platformio.ini (
  pio run -t clean
) else (
  echo Folder has no platformi.ini, skipping.
)
)
cd %back%