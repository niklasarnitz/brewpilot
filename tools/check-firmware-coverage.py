"""Fail on uncovered firmware, allowing only paths impossible in this build."""

import argparse
import json
import sys
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
models = ('RANCILIO_S27', 'LA_CIMBALI_M29_SELECT')
parser.add_argument('machine', choices=models)
parser.add_argument('flowmeters', choices=('0', '1'))
args = parser.parse_args()
src = Path(__file__).resolve().parents[1] / 'src'
data = json.load(sys.stdin)['data'][0]
files = {Path(f['filename']).relative_to(src): f for f in data['files']}

# New source files must appear in LLVM's inventory unless reviewed as declarative.
declarations = set(map(Path, '''StaticConfig.h UserConfig.h configs/MachineConfig.h
configs/machineConfigs/LaCimbaliM29Select.h configs/machineConfigs/RancilioS27.h
hardware/MachineType.h hardware/enums/GroupHeadButtonEvent.h hardware/state/State.h
hardware/state-handlers/GenericStateHandler.h structs/ButtonEvent.h structs/ButtonMatrixState.h
structs/VolumetricSettings.h'''.split()))
inactive = ('LaCimbaliM29ButtonMatrixHandler.h' if args.machine == models[0]
            else 'RancilioS27ButtonMatrixHandler.h')
unused = {Path('hardware/handlers/machine-specific-handlers') / inactive}
if args.machine == models[0]:
    unused.add(Path('utils/noop.h'))  # Only the M29 scanner needs settling time.
sources = {p.relative_to(src) for p in src.rglob('*')
           if p.suffix in {'.h', '.hpp', '.cpp', '.c', '.cc', '.ino'} and p.resolve().is_relative_to(src)}
expected = sources - declarations - unused
failures = [f'Missing coverage: {p}' for p in sorted(expected - files.keys())]
failures.extend(f'Unexpected source: {p}' for p in files.keys() - sources)

other_case = 'case MachineType::' + next(m for m in models if m != args.machine) + ':'
selectors = {'ButtonMatrixHandler.h', 'getMachineConfig.h'}
for path in sorted(sources & files.keys()):
    file = files[path]
    if path in unused:
        print(f'INACTIVE: {path}')
        continue
    name = path.name
    source = (src / path).read_text().splitlines()
    allowed = dict(lines=0, functions=0, regions=0, branches=0)
    post_save_branches = set()
    for fn in data['functions']:
        if Path(fn['filenames'][0]) != src / path:
            continue
        if name == 'VolumetricsHelper.h' and 'writeFlowMeterSetting' in fn['name']:
            post_save_branches.update(branch[0] for branch in fn['branches'])
        off_isr = (path == Path('main.cpp') and args.flowmeters == '0' and fn['name'] in
                   ('_Z24groupOneFlowMeterHandlerv', '_Z24groupTwoFlowMeterHandlerv'))
        if off_isr and not fn['count']:
            allowed['functions'] += 1
        for start, _, end, _, count, _, _, kind in fn['regions']:
            if kind != 0 or count:
                continue
            line = source[start - 1].strip()
            if (off_isr or (name in selectors and line == other_case) or
                    (name == 'getMachineConfig.h' and line == 'default:') or
                    (name == 'PreferenceHelper.h' and line == 'return "INVALID";')):
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
        # It can also fold one outcome of the constant machine switch entirely.
        if ((name in selectors and line in {other_case, f'case MachineType::{args.machine}:',
                                             'default:', 'switch (HARDWARE_MODEL)'}) or
                (max(yes, no) and ((name, line) in {
                    ('BoilerStateHandler.h', 'switch (internalState)'),
                    ('PreferenceHelper.h', 'switch (key)'),
                } or (start in post_save_branches and line == 'switch (event)')))):
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
