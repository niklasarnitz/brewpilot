# ESP32-Pin-Audit — BrewPilot

## Aktueller Stand: freies I2C und externe Eingangspull-ups

Die am 10.10.2026 genehmigte GPIO-/Pull-up-Revision ersetzt den zwischenzeitlichen
Rückbau. Sie ist in `pcb/BrewPilot.kicad_sch`, `pcb/BrewPilot.kicad_pcb` und
beiden Firmwarevarianten umgesetzt und lokal geprüft. Die Maschine wird jetzt
über ein eigenes PlatformIO-Buildziel gewählt, nicht durch manuelles Ändern
von `UserConfig.h`. Die Änderungen sind nicht geflasht oder als Fertigungsdaten
exportiert.

Die zwölf LaCimbali-Matrixsignale bleiben **zwölf Signale: sieben Eingänge und
fünf Scan-Ausgänge**. Zwei vorher unbenutzte Nur-Eingangs-GPIOs 36/39 übernehmen
die Eingänge von 13/14; dadurch können die Scanleitungen von 21/22 auf 13/14
wandern. Das schafft exklusives I2C auf 21/22, spart aber keine Matrixleitung.
Funktionen und Pinreihenfolge von J4/J5 sowie der externe Kabelbaum bleiben gleich.
Die Tastenauswertung wird durch die Umlegung nicht grundsätzlich verbessert.

**Alte PCB-Verdrahtung und neue Firmware sind inkompatibel.** Ein Softwareupdate
trennt die alten J5/J13-Verbindungen nicht. Ein Retrofit bräuchte physische
Leiterbahntrennungen und Neuverdrahtung; hier wird keiner ausgeführt. Die
Änderung stellt I2C elektrisch bereit, implementiert aber keinen `Wire`-Treiber
und verifiziert keinen I2C-Gerätebetrieb. Push, Flashen und Fertigung sind nicht
autorisiert.

## Firmware für beide Maschinen bauen

Voraussetzung: PlatformIO CLI `pio` im `PATH`. Vom Projektverzeichnis aus:

```sh
./tools/build-firmware.sh                 # beide Maschinen, kein Upload
pio run -e rancilio-s27                    # nur Rancilio S27
pio run -e la-cimbali-m29-select            # nur LaCimbali M29 Select
```

Das Skript funktioniert auch mit absolutem Pfad aus einem anderen Verzeichnis,
akzeptiert keine Argumente und gibt einen Buildfehler an den Aufrufer weiter.
Auch `pio run` ohne `-e` baut beide Ziele. Die Ergebnisse liegen getrennt unter
`.pio/build/rancilio-s27/` und `.pio/build/la-cimbali-m29-select/`, jeweils als
`firmware.bin` und `firmware.elf`. Diese Dateien ersetzen keinen vollständigen
Flashablauf mit Bootloader und Partitionstabelle.

Die Umgebungen setzen jeweils genau ein `BREWPILOT_MACHINE_*`-Makro;
`UserConfig.h` lehnt eine fehlende oder doppelte Auswahl beim Kompilieren ab.
Beide Ziele verwenden die oben beschriebene neue PCB-Pinbelegung. Das Skript
flasht nichts und führt keine Hardwaretests aus.

## Klemmen und Pull-Modi

Ansicht von der Bestückungsseite: J4/J5 oben, ESP32 darunter, USB nach rechts.
Die Klemmen zählen **von rechts nach links**. Nach aufgedruckten Pin-/IO-Nummern
verdrahten, nicht nach Kabelfarben oder einer Rückansicht. U1-Pads sind
BrewPilot-Symbol-/Footprintnummern, keine Chip-Padnummern.

```diagram
  J4 – IO1 bis IO6                 J5 – IO7 bis IO12
  Pin:   6  5  4  3  2  1         Pin:   6  5  4  3  2  1
  GPIO: 25 26 27 39 12 36         GPIO: 23 14 13 19 32 33
                      ESP32 darunter, USB →
```

`Iext` = `INPUT` mit externem 10-kΩ-Pull-up nach 3,3 V;
`I↑` = `INPUT_PULLUP` erst nach Boot; `O` = Ausgang.
„Groß/klein“ bezeichnet die gemeinsamen Eingänge für Doppel-/Einzeldosis.
„Stop“ wird als `continuous` ausgewertet. Die Bezeichnungen folgen der Firmware;
ein Original-Schaltbild der Maschinen-Tastaturen liegt nicht vor.

| Klemme | Flex-IO | GPIO / U1-Pad | LaCimbali M29 | Rancilio S27 |
|---|---|---|---|---|
| J4.1 | 1 | 36 / 3 | Iext: Gruppe 2 groß, R22 | Iext: Dose 4, R22 |
| J4.2 | 2 | 12 / 13 | I↑: Gruppe 2 klein | I↑: Continuous |
| J4.3 | 3 | 39 / 4 | Iext: Gruppe 2 Stop, R23 | Unbenutzt; R23 bleibt bestückt |
| J4.4 | 4 | 27 / 11 | Iext: Gruppe 1 groß, R27 | Iext: Dose 1, R27 |
| J4.5 | 5 | 26 / 10 | Iext: Gruppe 1 klein, R26 | Iext: Dose 2, R26 |
| J4.6 | 6 | 25 / 9 | Iext: Gruppe 1 Stop, R25 | Iext: Dose 3, R25 |
| J5.1 | 7 | 33 / 8 | O: Gruppe 1 links/Stop | O: Dosen/Continuous, dauerhaft LOW |
| J5.2 | 8 | 32 / 7 | O: Gruppe 1 rechts | Unbenutzt |
| J5.3 | 9 | 19 / 31 | O: Tee-Scan | O: Tee, dauerhaft LOW |
| J5.4 | 10 | 13 / 15 | O: Gruppe 2 links/Stop | Unbenutzt |
| J5.5 | 11 | 14 / 12 | O: Gruppe 2 rechts | Unbenutzt |
| J5.6 | 12 | 23 / 37 | Iext: Tee, R24 | Iext: Tee, R24 |

J13 gehört **nicht** zur Tastatur: Pin 1 = SDA/GPIO21/U1.33, Pin 2 =
SCL/GPIO22/U1.36, mit bestehenden 4,7-kΩ-Pull-ups R15/R16. Keine Verbindung
zu J5.4/J5.5. J13.3 und .5–.8 sind GND, .4 ist 3,3 V: nur zwei Signale,
keine weiteren freien GPIOs. J4/J5 sind 3,3-V-Logik; eine separat versorgte
5-/12-/24-V-Tastatur darf daraus nicht als direkt anschließbar abgeleitet werden.

### Verbindliche Pull-up-Entscheidung

- R22–R27: je 10 kΩ, Pad 1 an +3.3V, Pad 2 an F01/F03/F12/F06/F05/F04.
  GPIO36/39 besitzen keine internen Pulls und können nur Eingänge sein.
  GPIO23/25/26/27 verwenden ebenfalls externe Pull-ups und `INPUT`, in beiden
  Maschinenvarianten. Ein geschlossener Kontakt belastet den Scan-Ausgang
  je 10-kΩ-Pull-up zusätzlich mit etwa 0,33 mA. 36/39 werden abgefragt,
  nicht als Interruptquellen verwendet.
- GPIO12 behält ausschließlich den internen Pull-up nach Initialisierung.
  Ein permanent externer HIGH-Pegel kann beim Reset MTDI auf HIGH ziehen und
  ohne passende eFuses 1,8 V statt 3,3 V für VDD_SDIO wählen: Bootrisiko für
  3,3-V-Flash. Spätere `INPUT_PULLUP`-Aufrufe ändern eingelesene Straps nicht.
- Flowmeter GPIO16/J2.2 und GPIO35/J3.2: vorhandene 10 kΩ R9 beziehungsweise
  R8 nach 3,3 V; beide `INPUT` mit FALLING-Interrupts, auch bei S27.
- Boiler GPIO34/J10.2 bleibt `INPUT`/ADC mit R7-Pulldown und über R5 von
  GPIO17 getriebener Sondenanregung. Keine neue Pull-up-Beschaltung.
- SSR-Basis-Emitter-Pulldowns R17–R21 bleiben unverändert. Sie wirken gegen
  hochohmige/ schwach hochgezogene Eingänge, nicht gegen aktiv HIGH getriebene
  GPIOs; sie sind kein gemessener Reset-/Brownout-/Strap-Nachweis.

Die BOM zählt 64 elektrische Referenzen, davon 16 auf Lager und 48 zu kaufen.
R8/R9/R22–R27 bilden eine gemeinsame 10-kΩ-BOM-Zeile mit Menge 8:
Yageo `CFR-25JB-52-10K`, horizontaler DIN0207-THT-Footprint mit 7,62 mm Raster.
R5/R7 bleiben ihre separate 10-kΩ-Teileauswahl.

### Gemeinsame Pull-up-Reihe

Alle zehn Pull-ups für Matrix, I2C und Flowmeter stehen in einer geraden Reihe,
von links nach rechts: **R24, R16, R15, R22, R23, R8, R25, R26, R27, R9**.
Die Widerstandskörper verlaufen in der Draufsicht senkrecht, mit 4,8 mm Abstand
zwischen ihren Achsen. Die Versorgungspads liegen bei Y = 57 mm, die Signalpads
bei Y = 64,62 mm; die Reihenmitte liegt damit 3,38 mm über der vorherigen
Matrix-Pull-up-Reihe. `U1 ESP32` und `USB -->` stehen darunter bei Y = 70,5 mm.
Die SSR-Pulldowns gehören weiterhin zu ihren jeweiligen SSR-Baugruppen.
D6 rechts neben R9 ist ebenfalls auf die Padlinien Y = 57/64,62 mm ausgerichtet;
dafür wurde nur D6 um 0,69 mm nach oben verschoben. Seine X-Position bleibt
207,69 mm. Die lokale Routinganpassung benötigt keine zusätzlichen Vias.

Für diese Anordnung wurde das Niedervolt-Routing neu geführt. Pinbelegung,
Widerstandswerte und Schaltplan bleiben unverändert. Netzspannungsleiterbahnen,
SSR-Ausrichtung und die um 1 mm nach links verschobene Sicherung J11 bleiben
erhalten. Die aktuelle Position von `by ben & niklas` wurde übernommen.

## Kontaktpaare und Scan

Die folgenden **logischen Soll-Verbindungen** stammen aus den Tastenfunktionen
der Firmware, nicht aus einer nachgemessenen Original-Tastatur. Mögliche
Dioden/Elektronik werden dadurch nicht durch Drahtbrücken ersetzt.

| LaCimbali-Taste / Zustandsfeld | Gruppe 1: Eingang ↔ Scan | Gruppe 2: Eingang ↔ Scan |
|---|---|---|
| Links Einzeldosis / `leftSingleDose` | J4.5 ↔ J5.1 | J4.2 ↔ J5.4 |
| Links Doppeldosis / `leftDoubleDose` | J4.4 ↔ J5.1 | J4.1 ↔ J5.4 |
| Stop/Continuous / `continuous` | J4.6 ↔ J5.1 | J4.3 ↔ J5.4 |
| Rechts Einzeldosis / `rightSingleDose` | J4.5 ↔ J5.2 | J4.2 ↔ J5.5 |
| Rechts Doppeldosis / `rightDoubleDose` | J4.4 ↔ J5.2 | J4.1 ↔ J5.5 |

Tee: **J5.6 ↔ J5.3**, Zustandsfeld `tea`.

| S27-Taste | Eingang ↔ LOW-Rückleiter | Zustandsfeld in `groupOne` |
|---|---|---|
| Dose 1 | J4.4 ↔ J5.1 | `leftSingleDose` |
| Dose 2 | J4.5 ↔ J5.1 | `leftDoubleDose` |
| Dose 3 | J4.6 ↔ J5.1 | `rightSingleDose` |
| Dose 4 | J4.1 ↔ J5.1 | `rightDoubleDose` |
| Continuous | J4.2 ↔ J5.1 | `continuous` |

Bei S27 verwendet Tee ebenfalls J5.6 ↔ J5.3; `groupTwo` bleibt unbenutzt.
S27 nutzt sechs Eingänge und zwei dauerhaft LOW gesetzte Ausgänge.

LaCimbali setzt alle fünf Scan-Ausgänge nach Initialisierung HIGH. Pro
`handle()` wird jeweils eine Leitung LOW, kurz per `noop()` gewartet, gelesen
und danach wieder HIGH geschaltet. Reihenfolge:

1. J5.1 / GPIO33: Gruppe 1 links und Stop.
2. J5.2 / GPIO32: Gruppe 1 rechts.
3. J5.4 / GPIO13: Gruppe 2 links und Stop.
4. J5.5 / GPIO14: Gruppe 2 rechts.
5. J5.3 / GPIO19: Tee.

LOW wird zu `true` in `ButtonMatrixState`. Die Hauptschleife wartet nach ihrer
Arbeit 50 ms. `noop()` ist keine Entprellung; `ButtonHandler` erzeugt einen
Druck-Event beim Zustandswechsel und einen Halte-Event nach 40 gedrückten Samples.

**Inaktive LaCimbali-Scanleitungen sind aktiv HIGH, nicht hochohmig.** Beispiel:
links und rechts Einzeldosis der Gruppe 2 schließen beide nach J4.2. Ohne
Entkopplung verbinden sie GPIO13 LOW mit GPIO14 HIGH: Ausgangskonflikt, nicht
nur Ghosting. Diese bestehende Scanstrategie wird durch die Umlegung nicht
behoben. Erst anhand der echten Tastatur klären, ob Dioden/Elektronik den
Konflikt verhindern. Keine Scan-Ausgänge zusammenlegen. Bei S27 besteht dieser
konkrete HIGH/LOW-Konflikt zwischen den beiden LOW-Rückleitern nicht.

## Weitere belegte Pins und Hardwaregrenzen

| Funktion | GPIO / U1-Pad |
|---|---|
| SSR Gruppe 2 | 2 / 24 |
| SSR Gruppe 1 | 4 / 26 |
| SSR Pumpe | 5 / 29 |
| SSR Tee | 15 / 23 |
| SSR Boilerfüllung | 18 / 30 |
| Sondenanregung | 17 / 28 |
| Boiler ADC1_CH6 | 34 / 5 |
| Flowmeter Gruppe 1 / 2 | 16 / 27; 35 / 6 |

- U1 heißt `esp32`, Symbol `local:esp32_38_pins-custom_library`, Footprint
  `new_local:esp32_38pin`: DevKitC-artige 38-Pin-Belegung, aber kein Nachweis
  von Hersteller/Modul-/Chiprevision. `esp32doit-devkit-v1` im Buildprofil
  identifiziert die bestückte Hardware nicht. WROVER reserviert 16/17 für
  PSRAM und ist mit dieser Belegung nicht kompatibel. Modulaufschrift prüfen.
- GPIO0/U1.25 ist unbeschaltet, aber Boot-/Download-Strap, kein bedenkenlos
  freier Pin. GPIO1/U1.35 und GPIO3/U1.34 gehören zu UART0/USB-UART und
  Programmierung/Bootlogs. Der klassische ESP32 besitzt kein natives USB.
  GPIO6–11 sind für Flash reserviert, auch wenn am Träger unconnected.
  GPIO37/38 sind hier nicht herausgeführt; GPIO20/24/28–31 existieren nicht.
  36/39 sind jetzt belegt, kein weiterer uneingeschränkt freier LaCimbali-Ausgang.
- Straps des klassischen ESP32: 0/2/5/12/15, nicht GPIO4. Resetpegel hängen
  von Modul, Tastern und SSR-Basisnetzwerken ab. GPIO12/13/14/15 sind auch JTAG;
  diese Anwendung belegt sie. 34–39 sind input-only ohne interne Pulls.
- UART, Flowmeterinterrupts, Relais, Boiler-ADC und Flash/NVS bleiben erhalten.
  I2C ist eine Hardware-Erweiterungsoption, keine hier implementierte Peripherie.
- Alle GPIOs sind 3,3-V-Logik. Die akzeptierten Inputschutz-/Flowmeter-,
  SSR-/Installations- und RF-Entscheidungen stehen unverändert in
  [pcb/AGENTS.md](../pcb/AGENTS.md). Insbesondere begründen D6–D8 mit 5,1 V
  weder 5-V-Toleranz noch zuverlässigen Schutz direkter ESP32-Eingänge.

## Verifikation und Grenzen

Auf dem integrierten Projekt mit KiCad CLI 10.0.6 geprüft:

- ERC: **0 Verstöße**. DRC mit neu gefüllten Zonen und Schaltplanabgleich:
  **0 Fehler, 0 Warnungen, 0 offene Verbindungen, 0 Paritätsabweichungen**.
  Alle sieben zuvor ignorierten DRC-Prüfarten sind aktiviert; keine DRC-Ausnahmen.
  Feldwerte folgen dem Schaltplan. H1–H4 sind explizit rein mechanische
  PCB-Footprints, ohne Schaltplansymbol und ohne elektrische BOM-Position.
  Der ERC verwendet weiterhin sein bestehendes Profil mit sechs separat
  ignorierten Prüfarten (Raster, Vierfachknoten, einzelne globale Labels,
  Bibliothekssymbolabgleich, Footprintfilter und SPICE-Modelle); diese wurden
  durch die Aktivierung der sieben PCB-DRC-Prüfungen nicht verändert.
- Frischer Netlistabgleich: alle 64 elektrischen Referenzen, Werte und
  201 elektrischen Pad-Netze stimmen überein. Die sechs neuen Symbol-/Footprintverknüpfungen
  sind geprüft; ältere fehlende Verknüpfungen wurden nicht nebenbei verändert.
- Außer den zehn Pull-ups und D6 bleiben alle Bauteilpositionen erhalten. Pad-Geometrien,
  Pad-/Footprint-UUIDs, Bauteile und Texte der SSR-Gruppen, Netzspannungsrouting
  und Abstandsregeln sind erhalten.
- GND-Abstand zu fremdem Kupfer/Maskenöffnung mindestens 0,5003 mm;
  Netzspannung zu Niedervolt mindestens 6,0003 mm. Alle 28 geometrischen
  Massekomponenten sind über beide Lagen mit J1 verbunden.
- Beide PlatformIO-Varianten bauen erfolgreich. Der echte
  `ButtonMatrixHandler` erkennt im Hostmodell alle **2.048 M29-** und
  **64 S27-Kombinationen**, einschließlich Halten/Loslassen. Die GPIO-Zuordnung
  wurde unabhängig aus PCB-Klemmen und U1-Pinfunktionen gewonnen.
  Fehlende externe Pull-ups und ein deaktivierter interner GPIO12-Pull-up
  werden in Negativkontrollen erkannt. Das Modell setzt entkoppelte Kontakte
  voraus und bildet weder Prellen, Diodenspannung, Leitungs-RC noch Booten ab.
- Die fertige Bestückungsansicht wurde auf Bauteil-/Beschriftungsüberschneidungen
  kontrolliert. Der J5-Titel steht jetzt oberhalb des Klemmenkörpers.
  Die beiden Schriftzüge verwenden KiCad-Strichschrift mit definierten
  Strichbreiten; ihre Positionen bleiben erhalten.
- Die Warnungskorrektur lässt alle 68 Footprintpositionen, 207 Padpositionen/
  Bohrungen/Kupferformen und 802 Leiterbahn-/Via-Objekte unverändert. Jede
  SSR-Gruppe enthält jetzt auch beide LED-Anoden-Leiterbahnen: insgesamt
  6 Footprints, 11 lokale Leiterbahnen und 4 Beschriftungen je Gruppe.

Der U1-Courtyard umfasst die beiden Buchsenleisten auf der Trägerplatine mit
0,25 mm Randzugabe. Das darüber auf etwa 8 mm Abstand aufgesteckte ESP32-Modul
überspannt niedrige Bauteile; diese vertikale Staffelung ist Voraussetzung für
den zweigeteilten Courtyard, kein Nachweis des Freiraums für beliebige Module
oder höhere Bauteile. J13 nutzt einen projektlokalen Board-Edge-Footprint:
gleiche Pads, Bohrungen und Körperkontur wie der Amphenol-Standardfootprint,
aber auf die vorhandene Platinenfläche begrenzter Bestückungsdruck.

Nach dem Zusammenlegen der Pull-ups verwenden die betroffenen Netze
F01/F03/F10/F11 und I2C **3 Vias**, unter dem genehmigten Maximum von 5.
Die Gesamtplatine hat **36 statt zuvor 44 Vias**. Die Leiterbahnlänge dieser
sechs Netze beträgt einschließlich ihrer Pull-up-Abzweige rund 378,4 mm.
Die Neuverlegung ändert nicht die logischen Funktionen; reale Pegel und
Einschwingzeiten bleiben am Gerät zu prüfen.

Aktuelle Layoutprüfberichte, Netlist und Ansichten liegen lokal unter
`.amp/in/artifacts/warning-fix/`, einschließlich der erneuten Builds beider
Maschinenziele. Frühere Matrix-Hosttests liegen unter
`.amp/in/artifacts/pullups-final/`. `pcb/BrewPilot.net` ist ein alter Export,
keine aktuelle Verdrahtungsreferenz. **Kein Test an der echten Tastatur,
kein Reset-/Brownout-Test und kein I2C-Gerätetest wurde durchgeführt.**

Vor realem Betrieb:

1. Sicherstellen, dass die gefertigte Platine und die gewählte Firmwarevariante
   zu dieser Belegung und Bestückung gehören.
2. Spannungsfrei J4.1 → U1.3, J4.3 → U1.4, J5.4 → U1.15, J5.5 → U1.12
   prüfen; J5.4/J5.5 dürfen nicht direkt an J13.1/J13.2 liegen.
3. Nur im Niedervolt-Prüfaufbau mit abgetrennter Netzspannung und Lasten:
   Bestückung R22–R27 und Versorgung prüfen. Offene extern hochgezogene
   Eingänge müssen HIGH, aktive Kontakte in ihrer Scanphase LOW sein.
   GPIO12-Resetpegel separat prüfen; keinen permanenten Pull-up ergänzen.
4. Jede Taste einzeln drücken, halten, loslassen; ausschließlich das richtige
   Zustandsfeld muss reagieren. Scanpegel/Einschwingzeit mit Kabel prüfen.
5. Vor Mehrfachdruck die tatsächliche Kontaktentkopplung klären. Danach
   zulässige Kombinationen, Reset mit gedrückten Tasten, Brownout und Prellen
   prüfen. Ein Hostmodell mit entkoppelten Kontakten beweist keine elektrische
   Mehrfachtasten-Sicherheit und keine ESP32-Bootfähigkeit.
6. Gleichzeitige Matrixabfrage/I2C erst mit Treiber und angeschlossenem Gerät
   testen. Noch kein Flashen, kein ESP32-/Tastaturtest und keine Fertigungsfreigabe.

Ungeklärt bleiben genaue Modulvariante/eFuses, externe Tastatur-/LED-Schaltung,
Kabelbaum sowie reale Pegel bei Reset und Mehrfachdruck.

## Quellen

- Projekt: `pcb/BrewPilot.kicad_sch` (U1/J4/J5/J13/R8/R9/R15/R16/R22–R27),
  `src/StaticConfig.h`, beide maschinenspezifischen Matrixhandler,
  `ButtonMatrixHandler.h`, `main.cpp`, `BoilerStateHandler.h`, `ButtonHandler`.
  Layout: `pcb/BrewPilot.kicad_pcb`; aktuelle Prüfungen siehe oben.
- [ESP32 Series Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf):
  Pinübersicht, Flash/PSRAM, Straps/VDD_SDIO, GPIO34–39 ohne Pulls.
- [DevKitC V4 User Guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html):
  Header, USB-UART, Modulabhängigkeit GPIO16/17.
- [ESP-IDF GPIO API](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html):
  input-only Pins und Einschränkungen von GPIO36/39-Interrupts bei ADC/Funk/Sleep.
- [Arduino-ESP32 I2C API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html#setpins):
  Pinmux/`setPins` vor `begin`; Software-Pinmux ersetzt keine physische Trennung.
