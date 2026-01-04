# BrewPilot BLE Component Interaction Map

## Component Overview

```
┌───────────────────────────────────────────────────────────────────────┐
│                           CORE 0 (Main)                              │
├───────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐       │
│  │  InputHandler│      │  StateHandler│      │  StateActor  │       │
│  ├──────────────┤      ├──────────────┤      ├──────────────┤       │
│  │ • Buttons    │      │ • Boiler     │      │ • Relays     │       │
│  │ • Matrix     │      │ • Groups     │      │ • Solenoids  │       │
│  │ • Tea button │      │ • Tea        │      │              │       │
│  └──────────────┘      └──────────────┘      └──────────────┘       │
│       │                      │                      │                │
│       └──────────┬───────────┴──────────┬──────────┘                │
│                  │                      │                           │
│            ┌─────▼──────────┐           │                           │
│            │   ButtonEvent  │           │                           │
│            │   State struct │           │                           │
│            └────────────────┘           │                           │
│                  ▲                      │                           │
│                  │                      ▼                           │
│            ┌─────┴────────────────────────┐                         │
│            │   VolumetricsHelper          │                         │
│            │   PreferenceHelper           │                         │
│            │                              │                         │
│            │ [Shared with Core 1]         │                         │
│            └──────────────────────────────┘                         │
│                                                                       │
└───────────────────────────────────────────────────────────────────────┘
                            △
                            │ [Mutex Protected Access]
                            │
┌───────────────────────────┴───────────────────────────────────────────┐
│                           CORE 1 (BLE)                                │
├───────────────────────────────────────────────────────────────────────┤
│                                                                       │
│     ┌────────────────────────────────────────────┐                  │
│     │         BLECoreManager                     │                  │
│     ├────────────────────────────────────────────┤                  │
│     │ • Tracks state changes                     │                  │
│     │ • Updates BLE characteristics              │                  │
│     │ • Refreshes settings from preferences      │                  │
│     └────────────────────────────────────────────┘                  │
│                   │                                                  │
│                   ▼                                                  │
│     ┌────────────────────────────────────────────┐                  │
│     │         BLEService                         │                  │
│     ├────────────────────────────────────────────┤                  │
│     │ • 20+ GATT Characteristics                 │                  │
│     │ • Callbacks for writes                     │                  │
│     │ • Notifications for state changes          │                  │
│     └────────────────────────────────────────────┘                  │
│           │          │          │          │                        │
│     ┌─────┴────┬─────┴────┬─────┴────┬─────┴────┐                 │
│     ▼          ▼          ▼          ▼          ▼                   │
│   State      Volume      Backflush  Control  Diagnostics            │
│   Chars      Chars       Chars      Chars    Chars                  │
│                                                                       │
└───────────────────────────────────────────────────────────────────────┘
                            △
                            │ [BLE Advertisement]
                            │
                    ┌───────┴────────┐
                    │  Mobile App /   │
                    │  BLE Client     │
                    └────────────────┘
```

---

## Characteristic Ownership & Update Patterns

### STATE CHARACTERISTICS (Read + Notify)

Read whenever state changes, notify all connected clients

```
GroupHeadHandler.currentPulses ──┐
GroupHeadHandler.targetPulses   ├─→ BLECoreManager.updateProgress()
                                   │
                                   └─→ BLEService.pGroupOneProgress
                                       notify() ──→ Mobile Client
```

```
BoilerStateHandler.readBoilerProbe() ──┐
BoilerStateHandler.internalState      ├─→ BLECoreManager.updateBoilerState()
                                        │
                                        └─→ BLEService.pBoilerStateChar
                                            notify() ──→ Mobile Client
```

```
State.isFillingBoiler      ──┐
State.groupOneIsExtracting ├─→ BLECoreManager.updateState()
State.groupTwoIsExtracting │  │
State.isExtractingTeaWater ├─→ BLEService.pStateCharacteristic
State.isInProgrammingMode ─┘  │
                              notify() ──→ Mobile Client
```

---

### SETTINGS CHARACTERISTICS (Read + Write)

#### Volumetric Settings Flow

```
Mobile Client writes new dose
        │
        ▼
BLEService.pLeftSingleChar.onWrite()
        │
        ▼
VolumetricWriteCallback::onWrite()
        │
        ▼
PreferenceHelper.setULong(PreferenceKey::FlowMeterPulsesLeftSingleEspresso)
        │
        ▼
NVFlash Memory (stored persistently)
        │
        ▼
On next extraction:
GroupHeadHandler calls volumetricsHelper->getFlowMeterSetting()
        │
        ▼
Uses NEW value from VolumetricsHelper (updated in RAM)
```

#### Backflush Settings Flow

```
Mobile Client writes activation time
        │
        ▼
BLEService.pBackflushActivationChar.onWrite()
        │
        ▼
UInt16SettingCallback::onWrite()
        │
        ▼
PreferenceHelper.setULong(PreferenceKey::BackflushActivationTimeMs)
        │
        ▼
NVFlash Memory (stored persistently)
        │
        ▼
BLECoreManager periodically refreshes from preferences
        │
        ▼
Settings available next time backflush runs
```

---

### CONTROL CHARACTERISTICS (Write)

#### Programming Mode Control

```
Mobile Client: "Enable Programming Mode"
        │
        ▼
BLEService.pProgrammingModeChar.onWrite()
        │
        ▼
ProgrammingModeCallback::onWrite()
        │
        ▼
State.isInProgrammingMode = true
        │
        ▼
Core 0 reads flag:
GroupHeadHandler.handleState() ──→ enters programming logic
        │
        ▼
Next extraction records pulses instead of running for target
        │
        ▼
BLE notifies with new setting stored
```

---

## File Dependencies Chart

```
main.cpp
  │
  ├─→ BLECoreManager.h
  │     │
  │     └─→ BLEService.h
  │           │
  │           ├─→ State.h (read-only)
  │           ├─→ PreferenceHelper.h (for storage)
  │           └─→ NimBLEDevice/NimBLEServer (BLE stack)
  │
  ├─→ StateHandler.h
  │     ├─→ GroupHeadHandler.h
  │     │     └─→ VolumetricsHelper.h
  │     │           └─→ PreferenceHelper.h
  │     ├─→ BoilerStateHandler.h
  │     └─→ TeaStateHandler.h
  │
  ├─→ StateActor.h
  │     └─→ State.h
  │
  ├─→ InputHandler.h
  │
  ├─→ PreferenceHelper.h
  │     └─→ Preferences.h (Arduino)
  │
  └─→ VolumetricsHelper.h
        └─→ PreferenceHelper.h
```

**Key Insight**: BLECoreManager and BLEService are isolated from main control loop logic, only reading shared data structures.

---

## Real-time Data Update Frequencies

```
Core 0 (Main Loop)
├─ InputHandler.readInputs()           ───→ 20 Hz (every 50ms)
├─ StateHandler.handleState()          ───→ 20 Hz (every 50ms)
│  ├─ BoilerStateHandler               ───→ 0.5 Hz (every 2s)
│  ├─ GroupHeadStateHandler            ───→ 20 Hz (triggered by pulses)
│  └─ TeaStateHandler                  ───→ 20 Hz
└─ StateActor.loop()                   ───→ 20 Hz

Core 1 (BLE Loop)
├─ BLECoreManager.loop()               ───→ 10 Hz (every 100ms)
│  ├─ hasStateChanged()                ───→ Check every 100ms
│  ├─ updateState()                    ───→ Only if changed
│  ├─ Settings refresh                 ───→ Every 5 seconds
│  └─ Boiler characteristic update     ───→ Every 5 seconds
└─ NimBLE internal stack               ───→ Handles notifications

BLE Client (Mobile App)
└─ Receive notifications               ───→ ~10 Hz typical
                                            (depends on subscription)
```

**Strategy**:

- State changes → immediate notification (when detected)
- Settings → periodic update every 5 seconds
- Boiler → every 2 seconds (slow update) or periodically
- Progress → every 100ms check or on flow meter pulse

---

## Thread Safety Considerations

### Shared Data (No Mutex Needed - Single Writer)

- **State struct** (written by Core 0 only, read by Core 1)
- **VolumetricSettings** (written by Core 0 only, read by Core 1)
- **Boolean flags** (single-bit atomic)

### Shared Data (Preference Access - Needs Care)

- **PreferenceHelper.preferences** (accessed from both cores)
  - Core 0: Writes via setULong() during normal operation
  - Core 1: Reads via getULong() periodically
  - **Current**: No mutex, but infrequent access pattern is safe
  - **Future**: Consider adding mutex if frequency increases

### Isolated Data (No Sharing)

- **BLE Characteristics** (Core 1 only)
- **NimBLE Stack** (Core 1 only)
- **Relay/Solenoid objects** (Core 0 only)

---

## Future Enhancement: Bidirectional Control

```
Current (One-Way Control):
Mobile Client → (write) → BLE Characteristic → Preference → Core 0

Proposed (Remote Trigger):
Mobile Client → (write) → BLE Characteristic → Control Flag → Core 0
                                                               (triggers action)

Example: Trigger Auto-Backflush remotely:
1. Mobile app sends: "Start auto backflush on group 1"
2. BLE characteristic receives value
3. Control callback sets flag in ButtonEvent or State
4. Core 0 detects flag on next loop iteration
5. Executes backflush sequence
6. BLE notifies progress back to app
```

---

## Performance Analysis

### Memory Usage

```
BLE Service Characteristics:    ~2-4 KB (stack frame)
NimBLE Internal Buffers:        ~8-12 KB (configurable)
BLECoreManager Instance:        ~100 bytes
BLEService Instance:            ~100 bytes
─────────────────────────────────────────
Estimated Total:                ~10-16 KB of 320 KB available
Remaining for Core 1:           ~304+ KB (sufficient)
```

### CPU Impact

```
Core 0 (Main): No impact from Core 1 BLE operations
Core 1 (BLE):
  • Idle waiting: <1% CPU
  • Active BLE transfer: 5-10% CPU
  • Both cores combined: <15% CPU typical usage
```

### BLE Advertising Overhead

```
Advertisement packets:          100-200 bytes every 100ms
Data rate:                      1-2 KB/sec
Bandwidth:                      <5% of BLE capacity
```

---

## Recommended Monitoring Points

For a mobile app dashboard, monitor these characteristics:

```
┌─────────────────────────────────────┐
│     Real-time Machine Status        │
├─────────────────────────────────────┤
│ ☑ Machine State                     │
│   ├─ Boiler Filling: [YES|NO]       │
│   ├─ Group 1 Extracting: [YES|NO]   │
│   ├─ Group 2 Extracting: [YES|NO]   │
│   └─ Programming Mode: [YES|NO]     │
│                                     │
│ ☑ Group 1 Progress                 │
│   ├─ Pulses: 125 / 200              │
│   └─ Progress: [████░░░░░░░░░░░░░] │
│                                     │
│ ☑ Group 2 Progress                 │
│   ├─ Pulses: 250 / 250              │
│   └─ Progress: [██████████████████] │
│                                     │
│ ☑ Boiler Status                    │
│   ├─ Filling: [YES|NO]              │
│   └─ Probe: 980 (threshold: 1000)   │
│                                     │
│ ☑ Active Settings                   │
│   ├─ Left Single: 120 pulses        │
│   ├─ Right Double: 240 pulses       │
│   └─ Tea Duration: 5000ms           │
└─────────────────────────────────────┘
```

All with 100-500ms refresh rates via BLE notifications.
