# BrewPilot PCB BOM

This BOM was generated for one assembly from the current:

- `pcb/BrewPilot.kicad_sch`
- `pcb/BrewPilot.kicad_pcb`

The schematic contains 53 BOM references, and the PCB contains the same 53 references. No placed reference is missing from either file.

## Files

- `BrewPilot-master-bom.csv`: complete assembly BOM, including on-hand parts, bare PCB, socket strips, and fuse elements.
- `BrewPilot-buy-bom.csv`: procurement-only view with the user's on-hand relays, Phoenix connectors, ESP32, and socket strips removed.

## Quantity summary

- 53 schematic/PCB-referenced components
- 16 referenced components marked on hand: 10 Phoenix terminal blocks, 5 solid-state relays, and 1 ESP32 board
- 37 referenced components remaining to buy
- 2 on-hand ESP32 socket/header rows added as assembly-only items
- 2 required 5 × 20 mm fuse elements added as assembly-only items
- 1 bare PCB added as a fabrication item

## Unresolved selections

The two fuse holders are fully identified, but the actual fuse elements are not defined in the schematic:

- J11 is a Würth Elektronik 696108003002 holder for a 5 × 20 mm fuse on the solenoid AC phase.
- J12 is a SCHURTER 0031.8201 holder for a 5 × 20 mm fuse on the 5 V input.

Their current ratings and time characteristics must be selected from the real load/inrush, supply limit, conductor size, and applicable safety requirements. The BOM deliberately does not invent these ratings.

D6–D8 use Vishay `BZX55C5V1-TR` 5.1 V Zener diodes in the existing DO-35 through-hole footprints. They are pragmatic clamps for the machine-internal flow-meter and boiler-probe wiring; they are not IEC-rated ESD suppressors.

The five LEDs have no color specification in the schematic. Green `L-934GD` parts are proposed.

## Build-stopping checks

This is a complete count/sourcing BOM, not a release-to-production approval.

- KiCad 10.0.5 CLI ERC completes with 0 violations under the current project exclusions.
- KiCad 10.0.5 crashes while running CLI DRC on the current PCB (`Array index out of range`), so no valid current DRC result is available. Resolve the crash and obtain a clean DRC result before fabrication or mains testing.
- Several 3.81 mm Phoenix terminal blocks carry AC mains in the current netlist. Phoenix lists the selected MKDS 1 family at 200 V in overvoltage category III / pollution degree 2 and 400 V in category II / pollution degree 2. Confirm the actual installation category and required creepage/clearance before using them at 230 VAC.
- Omron discontinued `G3MB-202P DC5` in 2011. The on-hand relays should be known-authentic and verified; do not replenish them from an unverified marketplace.
- Confirm that the on-hand ESP32 is a 38-pin DevKitC-style board. A common 30-pin DOIT ESP32 DevKit V1 is not mechanically or electrically interchangeable with this footprint.

## Component references

- TDK `FG28C0G1H102JNT06`: https://product.tdk.com/en/search/capacitor/ceramic/lead-mlcc/info?part_no=FG28C0G1H102JNT06
- TDK `FG28X7R1H104KNT06`: https://product.tdk.com/en/search/capacitor/ceramic/lead-mlcc/info?part_no=FG28X7R1H104KNT06
- Panasonic FR electrolytic series: https://industrial.panasonic.com/cdbs/www-data/pdf/RDF0000/ABA0000C1259.pdf
- Vishay BZX55 series: https://www.vishay.com/docs/85604/bzx55.pdf
- Würth `696108003002`: https://www.we-online.com/components/products/datasheet/696108003002.pdf
- SCHURTER OGN / `0031.8201`: https://www.schurter.com/en/datasheet/typ_OGN.pdf
- Amphenol `54602-908LF`: https://www.amphenol-cs.com/product/54602908lf.html
- onsemi BC547 family: https://www.onsemi.com/download/data-sheet/pdf/bc550-d.pdf
- Espressif ESP32-DevKitC V4: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html
- Phoenix Contact 2-position block: https://www.phoenixcontact.com/da-dk/produkter/printkortklemme-mkds-1-2-381-1727010
- Phoenix Contact 3-position block: https://www.phoenixcontact.com/de-de/produkte/leiterplattenklemme-mkds-1-3-381-1727023
- Phoenix Contact 4-position block: https://www.phoenixcontact.com/en-gb/products/printed-circuit-board-terminal-mkds-1-4-381-1727036
- Phoenix Contact 6-position block: https://www.phoenixcontact.com/en-de/products/pcb-terminal-block-mkds-1-6-381-1727052
