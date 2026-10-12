#!/bin/sh
# Build both machines; never upload. Output: .pio/build/<environment>/firmware.bin
set -eu

if [ "$#" -ne 0 ]; then
    echo "Usage: $0 (builds both machines; no arguments accepted)" >&2
    exit 2
fi

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

if ! command -v pio >/dev/null 2>&1; then
    echo "Error: PlatformIO CLI 'pio' is required on PATH." >&2
    exit 127
fi

exec pio run --project-dir "$project_dir" \
    --environment rancilio-s27 \
    --environment la-cimbali-m29-select
