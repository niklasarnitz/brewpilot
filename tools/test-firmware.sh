#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build_dir=$(mktemp -d)
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM

coverage=0
case "${1:-}" in
    "") ;;
    --coverage) coverage=1 ;;
    *) echo "Usage: $0 [--coverage]" >&2; exit 2 ;;
esac

compiler=${CXX:-c++}
if [ "$coverage" = 1 ]; then
    compiler=${CXX:-clang++}
    if command -v xcrun >/dev/null 2>&1; then
        llvm_cov=${LLVM_COV:-$(xcrun --find llvm-cov)}
        llvm_profdata=${LLVM_PROFDATA:-$(xcrun --find llvm-profdata)}
    else
        llvm_cov=${LLVM_COV:-llvm-cov}
        llvm_profdata=${LLVM_PROFDATA:-llvm-profdata}
    fi
    coverage_dir=${COVERAGE_DIR:-$project_dir/.amp/in/artifacts/state-coverage}
    mkdir -p "$coverage_dir"
    python3 "$project_dir/test/coverage-gate.py"
fi

for machine in RANCILIO_S27 LA_CIMBALI_M29_SELECT; do
    for flowmeters in 0 1; do
        echo "Testing $machine, BREWPILOT_HAS_FLOWMETERS=$flowmeters"
        variant="$machine-flowmeters-$flowmeters"
        mkdir -p "$build_dir/$variant"
        binary="$build_dir/$variant/tests"
        set -- -std=c++17 -O2 -g -fsanitize=address,undefined \
            -DBREWPILOT_MACHINE_"$machine" -DBREWPILOT_HAS_FLOWMETERS="$flowmeters" \
            -I "$project_dir/test/firmware" -I "$project_dir/src" \
            "$project_dir/test/firmware/state-machine.cpp" -o "$binary"
        if [ "$coverage" = 1 ]; then
            set -- "$@" -O0 -fprofile-instr-generate -fcoverage-mapping
        fi
        "$compiler" "$@"
        LLVM_PROFILE_FILE="$build_dir/$variant/tests.profraw" "$binary"

        if [ "$coverage" = 1 ]; then
            report_dir="$coverage_dir/$variant"
            mkdir -p "$report_dir"
            "$llvm_profdata" merge -sparse "$build_dir/$variant/tests.profraw" -o "$build_dir/$variant/coverage.profdata"
            set -- "$binary" -instr-profile "$build_dir/$variant/coverage.profdata" \
                "$project_dir/src/hardware/state-handlers/BoilerStateHandler.h" \
                "$project_dir/src/hardware/state-handlers/GroupHeadHandler.h" \
                "$project_dir/src/hardware/state-handlers/StateHandler.h" \
                "$project_dir/src/hardware/state-handlers/TeaStateHandler.h" \
                "$project_dir/src/hardware/actors/StateActor.h"
            "$llvm_cov" report "$@" > "$report_dir/summary.txt"
            cat "$report_dir/summary.txt"
            "$llvm_cov" show "$@" -show-branches=count -format=html -output-dir="$report_dir/html"
            "$llvm_cov" export "$@" > "$report_dir/coverage.json"
            python3 "$project_dir/tools/check-state-coverage.py" "$report_dir/coverage.json"
        fi
    done
done
