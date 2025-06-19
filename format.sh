#!/bin/bash

# This script will look for every file in a program/ directory and enforce clang-format from .clang-format
find . -type f | grep -E '.*\/program\/.*\.(hpp|cpp)$' | while read -r file; do
    echo "Formatting file: $file"
    clang-format-19 -i --Werror $file
done 