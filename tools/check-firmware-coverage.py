"""Fail on uncovered firmware, allowing only paths impossible in this build."""

import argparse
import json
import sys
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
models = ('RANCILIO_S27', 'LA_CIMBALI_M29_SELECT')
parser.add_argument('machine', choices=models)
args = parser.parse_args()
src = Path(__file__).resolve().parents[1] / 'src'
data = json.load(sys.stdin)['data'][0]
files = {Path(f['filename']).relative_to(src): f for f in data['files']}

# Headers only declare; every executable line lives in a .cpp file that LLVM must report.
sources = {p.relative_to(src) for p in src.rglob('*')
           if p.suffix in {'.h', '.hpp', '.cpp', '.c', '.cc', '.ino'} and p.resolve().is_relative_to(src)}
headers = {p for p in sources if p.suffix in {'.h', '.hpp'}}
inactive = ('LaCimbaliM29ButtonMatrixHandler.cpp' if args.machine == models[0]
            else 'RancilioS27ButtonMatrixHandler.cpp')
unused = {Path('input/matrix') / inactive}
if args.machine == models[0]:
    unused.add(Path('utils/noop.cpp'))  # Only the M29 scanner needs settling time.
expected = sources - headers - unused
failures = [f'Missing coverage: {p}' for p in sorted(expected - files.keys())]
failures.extend(f'Unexpected source: {p}' for p in files.keys() - sources)
failures.extend(f'{p}: executable code in a header; move it to a .cpp file' for p in sorted(headers & files.keys())
                if files[p]['summary']['functions']['count'])

for path in sorted(sources & files.keys()):
    file = files[path]
    if path in unused:
        print(f'INACTIVE: {path}')
        continue
    name = path.name
    source = (src / path).read_text().splitlines()
    allowed = dict(lines=0, functions=0, regions=0, branches=0)
    for fn in data['functions']:
        if Path(fn['filenames'][0]) != src / path:
            continue
        # Absent groups' interrupt handlers are compiled but never attached.
        absent = (args.machine == models[0] and (path, fn['name']) in {
            (Path('main.cpp'), '_Z24groupTwoFlowMeterHandlerv'),
            (Path('state/StateHandler.cpp'), '_ZN12StateHandler31groupTwoFlowMeterPulseInterruptEv'),
        })
        if absent and not fn['count']:
            allowed['functions'] += 1
        for start, _, end, _, count, _, _, kind in fn['regions']:
            if kind != 0 or count:
                continue
            line = source[start - 1].strip()
            if absent:
                allowed['regions'] += 1
                allowed['lines'] += end - start + 1
                print(f'UNREACHABLE (not covered): {path}:{start}-{end} {line}')
            else:
                failures.append(f'{path}:{start}-{end}: uncovered region: {line}')
    for start, _, _, _, yes, no, *_ in file['branches']:
        if yes and no:
            continue
        line = source[start - 1].strip()
        # LLVM versions swap the two counters for synthetic switch defaults.
        if max(yes, no) and (name, line) in {
                ('BoilerStateHandler.cpp', 'switch (internalState)'),
                ('PreferenceHelper.cpp', 'switch (key)'),
                ('VolumetricsHelper.cpp', 'switch (event)'),
        }:
            allowed['branches'] += (yes == 0) + (no == 0)
            print(f'UNREACHABLE (not covered): {path}:{start} {line}')
        else:
            failures.append(f'{path}:{start}: uncovered branch: {line}')
    for metric, budget in allowed.items():
        counts = file['summary'][metric]
        if counts['count'] - counts['covered'] > budget:
            failures.append(f'{path}: {metric} {counts["covered"]}/{counts["count"]} ({budget} unreachable)')
    if path in expected and file['summary']['functions']['count'] == 0:
        failures.append(f'{path}: no functions instrumented')

if failures:
    raise SystemExit('\n'.join(failures))
print('PASS: all reachable firmware lines, functions, regions and branches; see raw LLVM counts above')
