#!/bin/sh
set -eu
cd "$(dirname "$0")"
cc -std=c99 -O2 -Wall -Wextra c_template_builder.c -o c_template_builder
printf '%s\n' '编译完成。运行 ./c_template_builder；查看版本用 --version。'
