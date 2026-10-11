# Firmware bedienen und konfigurieren

## Bauen und testen

```sh
./tools/build-firmware.sh  # beide Maschinen, kein Upload
./tools/test-firmware.sh   # Hosttests, C++17-Compiler mit ASan/UBSan erforderlich
```

Maschine über `rancilio-s27` beziehungsweise `la-cimbali-m29-select` wählen.
Pinbelegung und PCB-Kompatibilität stehen im [Pin-Audit](esp32-pin-audit.md).

## Flowmeter konfigurieren

`BREWPILOT_HAS_FLOWMETERS` ist standardmäßig `1`. Für Maschinen ohne Flowmeter
`-DBREWPILOT_HAS_FLOWMETERS=0` in die `build_flags` des jeweiligen
PlatformIO-Environments aufnehmen; das vorhandene Maschinen-Makro dabei beibehalten.

Beispiel für eine S27 **ohne Flowmeter**, als Ersatz des bestehenden Abschnitts:

```ini
[env:rancilio-s27]
build_flags =
    -DBREWPILOT_MACHINE_RANCILIO_S27
    -DBREWPILOT_HAS_FLOWMETERS=0
```

Die Flag `0` deaktiviert die Flowmeter-Interrupts. Fehlende oder ausbleibende
Flowmeter-Impulse lösen keinen eigenen Abbruch aus, unabhängig von dieser Flag.
Sie ersetzt keine Volumetrik durch eine Zeitdosierung: Ohne Sensoren Continuous
mit manuellem Stop verwenden. Gespeicherte Impulsdosen können ohne Impulse nicht
automatisch erreicht und neue Impulsdosen nicht gelernt werden. Ein Bezug über
eine bereits gespeicherte Dosistaste muss ohne Impulse manuell gestoppt werden.

Gruppenbezüge haben keine maximale Laufzeit: Dosierungen enden beim Erreichen
der gespeicherten Impulszahl oder durch Continuous; Continuous- und Lernbezüge
enden manuell. Das gilt auch über zwei Minuten hinaus und bei ausbleibenden
Impulsen. Rückspülen bleibt auf fünf Phasen à 10 s mit vier Pausen à 5 s begrenzt.
Normaler Tee behält seine gespeicherte Zeitdosierung.

Die Boilerfüllung hat keine maximale Laufzeit und keine Fehlersperre oder
Quittierung. Sie läuft, solange die Sonde „leer“ meldet, und endet nach einem
„voll“-Sample und der bestehenden Nachfüllverzögerung. Eine dauerhaft „leer“
meldende oder ausgefallene Sonde wird nicht durch einen Füllzeit-Timeout abgefangen.

## Gruppendosis programmieren

1. An einer inaktiven Gruppe Continuous drücken und etwa zwei Sekunden halten.
   Der kurz gestartete Bezug stoppt; **nur diese Gruppe** ist zum Programmieren
   reserviert und wartet auf die Auswahl einer ihrer vier Dosistasten.
2. Continuous loslassen und die gewünschte Dosistaste **derselben Gruppe** drücken.
   Dieser Bezug lernt die Impulszahl und ignoriert die bisher gespeicherte Dosis.
3. Mit Continuous **derselben Gruppe** stoppen und speichern. Die Programmierung
   endet damit. Vor der Dosisauswahl bricht diese Taste den Modus ohne Wasser ab.

Die andere Gruppe arbeitet normal weiter. Ihre Dosistasten wählen keinen
Lernbezug aus, und ihre Continuous-Taste kann die reservierte Gruppe weder
übernehmen noch deren Dosis speichern. Die vier Impuls-Dosiseinstellungen
bleiben gemeinsam: Eine gespeicherte Dosis gilt anschließend für beide Gruppen.

## Tee programmieren

1. **Tee** drücken und etwa zwei Sekunden halten. Ein eventuell laufender
   Teebezug stoppt; Tee ist jetzt zum Programmieren reserviert.
2. Tee loslassen und erneut kurz drücken. Dieser neue Bezug lernt die Dauer
   und ignoriert die bisher gespeicherte Teezeit.
3. Tee erneut drücken, um zu stoppen, die Dauer zu speichern und den Modus zu verlassen.

Das funktioniert auch ohne gespeicherte Teezeit oder wenn diese kürzer als die
Haltezeit ist. Ein Continuous-Hold an einer Gruppe aktiviert keine Teeprogrammierung.

Es kann nur eine Programmiersitzung gleichzeitig geben, auch während der
Dosisauswahl: Ein Hold an einer anderen Gruppe oder an Tee übernimmt keine
bereits reservierte Sitzung. Andere normale Bezüge können weiterhin gestartet
und unabhängig gestoppt werden.

Nullwerte und fehlgeschlagene NVS-Schreibvorgänge ersetzen keine aktive
Einstellung; der Lernbezug endet trotzdem, die Fehlermeldung erscheint auf Serial.
Nach einem Speicherfehler erneut programmieren.

Stop hat Vorrang vor der Rückspülkombination. Während Stop gehalten bleibt,
werden weitere Dosis-/Rückspülstarts an dieser Gruppe unterdrückt.
