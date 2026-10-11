#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build_dir=$(mktemp -d)
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM

for machine in RANCILIO_S27 LA_CIMBALI_M29_SELECT; do
    for flowmeters in 0 1; do
        echo "Testing $machine, BREWPILOT_HAS_FLOWMETERS=$flowmeters"
        "${CXX:-c++}" -std=c++17 -O2 -g -fsanitize=address,undefined \
            -DBREWPILOT_MACHINE_"$machine" -DBREWPILOT_HAS_FLOWMETERS="$flowmeters" \
            -I "$project_dir/test/firmware" -I "$project_dir/src" \
            "$project_dir/test/firmware/main.cpp" -o "$build_dir/tests"
        "$build_dir/tests"
    done
done
