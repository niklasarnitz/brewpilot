"""Validate an LLVM coverage export without changing its raw coverage counts."""

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
        # switch. LLVM 18 and newer LLVM versions report its zero counter on
        # opposite sides. Keep raw counts unchanged and tolerate only this
        # switch with exactly one executed and one unexecuted outcome.
        if (name == "BoilerStateHandler.h" and min(branch[4:6]) == 0 and max(branch[4:6]) > 0 and
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
