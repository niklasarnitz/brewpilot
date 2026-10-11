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
            python3 - "$report_dir/coverage.json" <<'PY'
import json
import sys
from pathlib import Path

with open(sys.argv[1]) as report:
    files = json.load(report)["data"][0]["files"]
expected = {"BoilerStateHandler.h", "GroupHeadHandler.h", "StateHandler.h", "TeaStateHandler.h", "StateActor.h"}
seen = set()
failures = []
for file in files:
    name = file["filename"].rsplit("/", 1)[-1]
    if name not in expected:
        continue
    seen.add(name)
    for metric in ("lines", "functions", "regions"):
        counts = file["summary"][metric]
        if counts["covered"] != counts["count"]:
            failures.append(f'{name}: {metric} {counts["covered"]}/{counts["count"]}')
    unreachable = 0
    source_lines = Path(file["filename"]).read_text().splitlines()
    for branch in file["branches"]:
        # LLVM counts a synthetic default for the exhaustive private BoilerState
        # switch. No public transition can create an out-of-enum value. Keep the
        # raw report unchanged and tolerate ONLY this switch's false outcome.
        if (name == "BoilerStateHandler.h" and branch[4] > 0 and branch[5] == 0 and
                source_lines[branch[0] - 1].strip() == "switch (internalState)"):
            unreachable += 1
            print(f"UNREACHABLE (not covered): {name}:{branch[0]} implicit switch default")
    counts = file["summary"]["branches"]
    if unreachable > 1 or counts["covered"] + unreachable != counts["count"]:
        failures.append(f'{name}: branches {counts["covered"]}/{counts["count"]} ({unreachable} unreachable)')
failures.extend(f"Missing coverage: {name}" for name in sorted(expected - seen))
if failures:
    sys.exit("Reachable coverage must be 100%:\n" + "\n".join(failures))
print("PASS: 100% state-machine lines, functions, regions and reachable branches; raw branches above")
PY
        fi
    done
done
