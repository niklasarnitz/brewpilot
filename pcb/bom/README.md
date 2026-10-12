# BrewPilot PCB BOM

This BOM covers one assembly of the approved GPIO/pull-up revision:

- Schematic: `pcb/BrewPilot.kicad_sch`
- Layout: `pcb/BrewPilot.kicad_pcb`

The schematic and PCB contain the same 64 electrical BOM references, verified against a fresh netlist. The PCB also contains four mechanical mounting holes.

## Files

- `BrewPilot-master-bom.csv`: complete assembly BOM, including on-hand parts, bare PCB, socket strips, and fuse elements.
- `BrewPilot-buy-bom.csv`: procurement-only view with the user's on-hand relays, Phoenix connectors, ESP32, and socket strips removed.

## Quantity summary

- 64 schematic-referenced electrical components
- 16 referenced components marked on hand: 10 Phoenix terminal blocks, 5 solid-state relays, and 1 ESP32 board
- 48 referenced components remaining to buy
- 2 on-hand ESP32 socket/header rows added as assembly-only items
- 2 required 5 × 20 mm fuse elements added as assembly-only items
- 1 bare PCB added as a fabrication item

R17–R21 add one 4.7 kΩ base-emitter pulldown to Q1–Q5 respectively, using the same resistor part and horizontal footprint as the existing 4.7 kΩ resistors. These bias the transistor bases low for high-impedance GPIOs with weak internal pull-ups; they do not override an actively driven HIGH. Physical boot/reset and brownout testing remains necessary.

The approved revision supersedes the interim revert: J4.1/J4.3 use GPIO36/39,
J5.4/J5.5 use GPIO13/14, and J13 independently uses GPIO21/22 with existing
4.7 kΩ R15/R16. R22–R27 are 10 kΩ pull-ups to 3.3 V on GPIO36/39/23/25/26/27.
Together with flowmeter pull-ups R8/R9, this is eight identical
`CFR-25JB-52-10K` parts, in the existing horizontal 7.62 mm THT footprint.
These externally biased inputs use `INPUT` in both machine variants. GPIO12
alone retains a post-boot internal pull-up: a permanent external high risks
the MTDI 1.8 V flash strap. Boiler GPIO34/R7/R5 remain unchanged.
Choose the machine through the `rancilio-s27` or `la-cimbali-m29-select`
PlatformIO environment; `tools/build-firmware.sh` builds both without uploading.
Old PCB wiring and new firmware are incompatible. See
[the current pin assignment and build commands](../../docs/esp32-pin-audit.md).

## Unresolved selections

The PCB uses BLX-A footprints for both fuse holders, based on the listing's 22.2 mm pin pitch and 27.8 × 9.5 mm cover dimensions. Confirm the delivered parts' pin size, pitch, and electrical approvals before assembly. The actual fuse elements are not defined in the schematic:

- J11 is a covered BLX-A holder for a 5 × 20 mm fuse on the solenoid AC phase; verify mains suitability.
- J12 is a covered BLX-A holder for a 5 × 20 mm fuse on the 5 V input.

Their current ratings and time characteristics must be selected from the real load/inrush, supply limit, conductor size, and applicable safety requirements. The BOM deliberately does not invent these ratings.

D6–D8 use Vishay `BZX55C5V1-TR` 5.1 V Zener diodes in the existing DO-35 through-hole footprints. They are pragmatic clamps for the machine-internal flow-meter and boiler-probe wiring; they are not IEC-rated ESD suppressors.

The five LEDs have no color specification in the schematic. Green `L-934GD` parts are proposed.

## Build-stopping checks

This is a complete count/sourcing BOM, not a release-to-production approval.

- R17–R21 remain fitted. KiCad 10.0.6 CLI ERC reports 0 violations on the
  integrated project. Filled-zone DRC reports 0 errors, 0 warnings,
  0 open connections and 0 schematic-parity issues, with all seven formerly
  ignored checks enabled and no DRC exclusions. Independent netlist comparison
  confirms every electrical pad net and component value.
- All ten pull-ups now share one row, with unchanged values and quantities.
  Current routing measurements are in [the pin audit](../../docs/esp32-pin-audit.md);
  current layout checks and both machine-target builds are in
  `.amp/in/artifacts/warning-fix/`. Earlier isolated-contact host tests remain in
  `.amp/in/artifacts/pullups-final/`. These checks do not replace real
  keyboard/reset testing. J13's local board-edge footprint changes only silk
  extent, not its pads or the ordered connector. U1's two-part courtyard assumes
  the confirmed elevated module and PCB-level socket strips.
- Several 3.81 mm Phoenix terminal blocks carry AC mains in the current netlist. Phoenix lists the selected MKDS 1 family at 200 V in overvoltage category III / pollution degree 2 and 400 V in category II / pollution degree 2. Confirm the actual installation category and required creepage/clearance before using them at 230 VAC.
- Omron discontinued `G3MB-202P DC5` in 2011. The on-hand relays should be known-authentic and verified; do not replenish them from an unverified marketplace.
- Confirm that the on-hand ESP32 is a 38-pin DevKitC-style board. A common 30-pin DOIT ESP32 DevKit V1 is not mechanically or electrically interchangeable with this footprint.

## Component references

- TDK `FG28C0G1H102JNT06`: https://product.tdk.com/en/search/capacitor/ceramic/lead-mlcc/info?part_no=FG28C0G1H102JNT06
- TDK `FG28X7R1H104KNT06`: https://product.tdk.com/en/search/capacitor/ceramic/lead-mlcc/info?part_no=FG28X7R1H104KNT06
- Panasonic FR electrolytic series: https://industrial.panasonic.com/cdbs/www-data/pdf/RDF0000/ABA0000C1259.pdf
- Vishay BZX55 series: https://www.vishay.com/docs/85604/bzx55.pdf
- BLX-A listing and mechanical drawing: https://de.aliexpress.com/item/1005008894822999.html
- Amphenol `54602-908LF`: https://www.amphenol-cs.com/product/54602908lf.html
- onsemi BC547 family: https://www.onsemi.com/download/data-sheet/pdf/bc550-d.pdf
- Espressif ESP32-DevKitC V4: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html
- Phoenix Contact 2-position block: https://www.phoenixcontact.com/da-dk/produkter/printkortklemme-mkds-1-2-381-1727010
- Phoenix Contact 3-position block: https://www.phoenixcontact.com/de-de/produkte/leiterplattenklemme-mkds-1-3-381-1727023
- Phoenix Contact 4-position block: https://www.phoenixcontact.com/en-gb/products/printed-circuit-board-terminal-mkds-1-4-381-1727036
- Phoenix Contact 6-position block: https://www.phoenixcontact.com/en-de/products/pcb-terminal-block-mkds-1-6-381-1727052
