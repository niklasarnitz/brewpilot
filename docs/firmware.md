# Firmware bedienen und konfigurieren

## Bauen und testen

```sh
./tools/build-firmware.sh  # beide Maschinen, kein Upload
./tools/test-firmware.sh   # Hosttests, C++17-Compiler mit ASan/UBSan erforderlich
```

Maschine über `rancilio-s27` beziehungsweise `la-cimbali-m29-select` wählen.
Pinbelegung und PCB-Kompatibilität stehen im [Pin-Audit](esp32-pin-audit.md).

## Schutzgrenzen

Diese Defaults sind nicht an der Maschine vermessen. Insbesondere die maximale
Boilerfüllzeit muss zur Kesselgröße und tatsächlichen Füllleistung passen.
Overrides kommen als `-DNAME=WERT` in die `build_flags` des jeweiligen
PlatformIO-Environments; das vorhandene Maschinen-Makro dabei beibehalten.

| Flag | Default | Bedeutung |
|---|---:|---|
| `BREWPILOT_HAS_FLOWMETERS` | `1` | `0` für Maschinen ohne Flowmeter |
| `BREWPILOT_NO_FLOW_TIMEOUT_MS` | `10000` | Höchstens 10 s ohne beobachteten Impuls während eines Gruppenbezugs |
| `BREWPILOT_MAX_EXTRACTION_MS` | `120000` | Höchstens 120 s pro Gruppenbezug, auch Continuous und Lernen |
| `BREWPILOT_MAX_BOILER_FILL_MS` | `120000` | Höchstens 120 s je zusammenhängender Boilerfüllung, einschließlich Nachfüllverzögerung |

Beispiel für eine S27 **ohne Flowmeter**, als Ersatz des bestehenden Abschnitts:

```ini
[env:rancilio-s27]
build_flags =
    -DBREWPILOT_MACHINE_RANCILIO_S27
    -DBREWPILOT_HAS_FLOWMETERS=0
```

Die Flag `0` deaktiviert ausschließlich die Impulsausfallüberwachung und die
Flowmeter-Interrupts, **nicht** die maximale Bezugs- oder Boilerfüllzeit.
Sie ersetzt keine Volumetrik durch eine Zeitdosierung: Ohne Sensoren Continuous
mit manuellem Stop verwenden. Gespeicherte Impulsdosen können ohne Impulse nicht
automatisch erreicht und neue Impulsdosen nicht gelernt werden. Ein Bezug über
eine bereits gespeicherte Dosistaste endet dann durch Stop oder die Zeitgrenze.

Ein Gruppen-Timeout beendet den Bezug ohne Speichern einer Lerndosis. Ein neuer
Tastendruck kann einen neuen Bezug starten. Rückspülen verwendet wegen des
Blindsiebs keine Impulsüberwachung; es bleibt auf fünf Phasen à 10 s mit vier
Pausen à 5 s begrenzt. Tee behält seine gespeicherte Zeitdosierung; die
Gruppen-Zeitgrenzen gelten nicht für Tee.

Die Grenzen werden in der Hauptschleife geprüft, nicht von einer unabhängigen
Hardwareabschaltung. Die Abschaltung erfolgt beim nächsten Schleifendurchlauf;
zwischen Prüfungen liegen regulär 50 ms plus die Laufzeit der Schleifenarbeit.
Blockierte Firmware, klebende Relais und die Heizung werden damit nicht abgesichert.

## Boiler-Füllfehler quittieren

Bei Überschreiten der Füllzeit werden Pumpe, Füllventil, beide Gruppen und Tee
ausgeschaltet. Rückspülen und Programmierung werden verworfen. Die Sperre bleibt
auch dann bestehen, wenn die Sonde später wieder „voll“ meldet; normale
Bezugstasten lösen keinen Neustart aus. Der Fehler wird über Serial gemeldet.

Erst die Ursache prüfen. Dann Continuous an einer Gruppe loslassen, **neu drücken
und etwa zwei Sekunden halten**. Eine beim Fehler bereits gehaltene Taste zählt
nicht. Die Quittierung startet weder einen Bezug noch den Programmiermodus.
Anschließend wird die Sonde erneut gemessen; ist der Kessel noch leer, beginnt
ein neuer, wieder zeitlich begrenzter Füllversuch.

Die Verriegelung liegt im RAM: Ein Neustart löscht sie ebenfalls. Es gibt keine
automatischen Wiederholungen während einer verriegelten Sitzung.

## Dosis oder Tee programmieren

1. An einer inaktiven Gruppe Continuous drücken und etwa zwei Sekunden halten.
   Der kurz gestartete Bezug stoppt; die Firmware wartet auf eine Auswahl.
2. Continuous loslassen und die gewünschte Dosistaste an einer inaktiven Gruppe
   oder Tee bei inaktivem Teebezug drücken. Nur dieser neue Bezug lernt.
3. Gruppen-Lernbezug mit dessen Continuous-Taste, Tee mit erneutem Tee-Druck
   beenden. Andere laufende Bezüge behalten ihre Abschaltgrenzen und können
   unabhängig gestoppt werden.

Vor der Auswahl kann Continuous an einer inaktiven Gruppe die Programmierung
abbrechen, ohne Wasser zu starten oder eine alte Dosis zu überschreiben.
Nullwerte und fehlgeschlagene NVS-Schreibvorgänge ersetzen keine aktive
Einstellung; der Lernbezug endet trotzdem, die Fehlermeldung erscheint auf Serial.
Nach einem Speicherfehler erneut programmieren. Die vorhandenen vier
Impuls-Dosiseinstellungen werden weiterhin von beiden Gruppen gemeinsam genutzt.

Stop hat Vorrang vor der Rückspülkombination. Während Stop gehalten bleibt,
werden weitere Dosis-/Rückspülstarts an dieser Gruppe unterdrückt.
