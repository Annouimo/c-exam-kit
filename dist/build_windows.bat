@echo off
cd /d "%~dp0"
set "scaffold_source="
set /a scaffold_count=0
for %%F in (c_scaffold_v*.c) do (
  set "scaffold_source=%%F"
  set /a scaffold_count+=1 >nul
)
if not "%scaffold_count%"=="1" (
  echo Expected exactly one versioned c_scaffold_v*.c file.
  pause
  exit /b 1
)
gcc -std=c99 -O2 -Wall -Wextra "%scaffold_source%" -o c_scaffold.exe -luser32
if errorlevel 1 (
  echo Compilation failed. Check GCC / MinGW and PATH.
) else (
  echo Compiled. Run c_scaffold.exe.
)
pause
