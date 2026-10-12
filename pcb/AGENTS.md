# PCB review decisions

This is a hobby project. Review technical function, fabrication and practical
electrical risks; regulatory certification is outside the requested scope.

## Accepted configuration

Niklas confirmed these decisions on 2026-10-10 in
[the PCB review](https://ampcode.com/threads/T-01a12672-2508-706a-a29c-2bf588367e3a).
Treat them as project decisions for the existing hardware and installation,
not as independently measured results or guarantees for other configurations.

- **Input protection and flowmeters:** The owner explicitly accepts D6–D8 as
  fitted (5.1 V Zeners), the direct ESP32 input connections without series
  current limiting, and the discrepancy between the 3.3 V supply at J2/J3 and
  the 5 V flowmeter wiring note. These remain known electrical limitations:
  the clamps do not establish 5 V tolerance or reliable protection of 3.3 V
  GPIOs. Preserve this design unless a change is requested. Report it as an
  accepted risk, rather than repeatedly blocking the unchanged design on it.
- **SSRs and protection:** The owner reports that the existing G3MB-202P relay,
  load and protection arrangement has been tested and works for this machine.
  Accept it for that installation, including the absence of a separate PCB
  pump fuse. Exact load measurements, fuse ratings, external protection
  details and test conditions were not supplied. Do not invent them or
  describe the datasheet limits as changed. Reassess if loads, supply voltage,
  protection, ventilation or installation change.
- **Assembly and service:** The owner confirms the ESP32 module/BLX-A holder
  fit and existing power/service arrangement are suitable. Treat these as
  confirmed for the parts and arrangement in use, not for interchangeable
  ESP32 boards or arbitrary simultaneous USB/external powering.
- **Antenna clearance:** The owner accepts the existing placement of the
  PCB-antenna module above the carrier on approximately 8 mm headers inside
  the metal machine, including potentially poor WiFi/Bluetooth performance.
  Preserve the current copper and placement unless an RF change is requested.
  Header height does not establish that the antenna clearance is adequate;
  this is an accepted performance tradeoff, not a measured RF result.

The relay reset/off-state and mounting hardware clearance are separate issues,
not covered by these acceptances.

## Approved matrix / I2C revision

The approved 2026-10-10 revision supersedes the interim pin revert:
J4.1/J4.3 use GPIO36/39; J5.4/J5.5 use GPIO13/14. J13 alone uses GPIO21/22,
with existing 4.7 kΩ pull-ups R15/R16. Fit 10 kΩ pull-ups R22–R27 on
GPIO36/39/23/25/26/27 and use `INPUT` in both machine variants. GPIO12
retains `INPUT_PULLUP` only after boot; a permanent external pull-up risks
the MTDI flash-voltage strap. Flow inputs 16/35 use existing R9/R8 and `INPUT`.
Keep boiler GPIO34/R7/R5, SSR pulldowns R17–R21, centered assemblies and
clearance rules unchanged. The approved via budget for F01/F03/F10/F11 and
I2C together is at most 5 rather than the original 4. The ten pull-ups
R8/R9/R15/R16/R22–R27 now share one straight row; its placement and current
routing measurements are recorded in `docs/esp32-pin-audit.md`.

For matrix wiring or pull-mode changes, consult `docs/esp32-pin-audit.md` and
update both machine variants with the hardware. Select the machine through its
PlatformIO environment; `tools/build-firmware.sh` builds both without uploading.
Old PCB wiring and new firmware are incompatible. The revision is integrated
in `pcb/BrewPilot.kicad_sch` and `pcb/BrewPilot.kicad_pcb`.
The pin audit records checks and their limits; logical contact-model tests
are not tests of the actual keyboard, boot behavior or I2C device operation.
Push, flash and fabrication require separate owner authorization.

## Layout validation

Keep all DRC checks enabled. H1–H4 are PCB-only mechanical mounting holes.
U1's two courtyards cover the PCB-level socket strips, assuming the confirmed
elevated module; reassess this geometry if header height or under-module parts
change. J13's local BoardEdge footprint preserves standard pads and mechanics
while clipping silk to this board. For mechanical changes, read the verification
section of `docs/esp32-pin-audit.md` for these assumptions and check limits.

## Fabrication exports

The obsolete May, June and July 2026 fabrication exports were removed at the
owner's request. Generate Gerbers and drill files together from the intended
current KiCad revision when a new fabrication export is requested. Historical
exports in Git or backup directories are not release files.
