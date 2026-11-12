@echo off
chcp 65001 >nul
cd /d "%~dp0"
gcc -std=c99 -O2 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 c_template_builder.c -o c_template_builder.exe -luser32
if errorlevel 1 (
  echo 编译失败，请检查 GCC / MinGW 是否安装并加入 PATH。
) else (
  echo 编译完成，运行 c_template_builder.exe。
)
pause
