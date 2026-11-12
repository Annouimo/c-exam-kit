#!/bin/sh
set -eu
cd "$(dirname "$0")"
set -- c_scaffold_v*.c
if [ "$#" -ne 1 ] || [ ! -f "$1" ]; then
    printf '%s\n' 'Expected exactly one versioned c_scaffold_v*.c file.' >&2
    exit 1
fi
cc -std=c99 -O2 -Wall -Wextra "$1" -o c_scaffold
printf '%s\n' 'Compiled. Run ./c_scaffold; use --version to check the version.'
