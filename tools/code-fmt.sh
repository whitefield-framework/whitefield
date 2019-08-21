#!/bin/bash

FMT=clang-format-9

function handle_file()
{
    $FMT -i -style=file $1
}

function handle_dir()
{
    for f in `find "$1" \( -name "*.[ch]" -or -name "*.cpp" -or -name "*.hpp" \)`; do
        handle_file "$f"
    done
}

which $FMT > /dev/null
[[ $? -ne 0 ]] && echo "Install $FMT" && exit
[[ -d "$1" ]] && handle_dir "$1"
[[ -f "$1" ]] && handle_file "$1"
