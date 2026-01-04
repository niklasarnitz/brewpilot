# BrewPilot BLE Feature Matrix

## Quick Reference Table

| Category | Feature | Status | Priority | Data Type | Affected Files |
|----------|---------|--------|----------|-----------|-----------------|
| **STATE** | Machine State | ✅ Done | High | 5 bools | BLEService.h |
| **STATE** | Boiler Fill Status | ❌ Not Done | High | bool | BoilerStateHandler.h |
| **STATE** | Boiler Probe Raw Value | ❌ Not Done | Medium | uint16 | BoilerStateHandler.h |
| **STATE** | Group 1 Progress (pulses) | ❌ Not Done | High | uint32 | GroupHeadHandler.h |
| **STATE** | Group 2 Progress (pulses) | ❌ Not Done | High | uint32 | GroupHeadHandler.h |
| **SETTINGS** | Backflush Activation Time | ✅ Done | High | uint16 | PreferenceHelper.h |
| **SETTINGS** | Backflush Deactivation Time | ✅ Done | High | uint16 | PreferenceHelper.h |
| **SETTINGS** | Left Single Espresso | ❌ Not Done | High | uint32 | VolumetricsHelper.h |
| **SETTINGS** | Left Double Espresso | ❌ Not Done | High | uint32 | VolumetricsHelper.h |
| **SETTINGS** | Right Single Espresso | ❌ Not Done | High | uint32 | VolumetricsHelper.h |
| **SETTINGS** | Right Double Espresso | ❌ Not Done | High | uint32 | VolumetricsHelper.h |
| **SETTINGS** | Tea Water Duration | ❌ Not Done | High | uint32 | VolumetricsHelper.h |
| **SETTINGS** | Auto Backflush Extract Duration | ❌ Not Done | Medium | uint16 | GroupHeadHandler.h |
| **SETTINGS** | Auto Backflush Pause Duration | ❌ Not Done | Medium | uint16 | GroupHeadHandler.h |
| **SETTINGS** | Auto Backflush Cycle Count | ❌ Not Done | Medium | uint8 | GroupHeadHandler.h |
| **CONTROL** | Programming Mode Enable/Disable | ❌ Not Done | Medium | bool | State.h |
| **CONTROL** | Manual Trigger Auto Backflush | ❌ Not Done | Low | trigger | GroupHeadHandler.h |
| **RELAY** | Pump Relay State | ❌ Not Done | Low | bool | StateActor.h |
| **RELAY** | Boiler Fill Solenoid | ❌ Not Done | Low | bool | StateActor.h |
| **RELAY** | Group 1 Solenoid | ❌ Not Done | Low | bool | StateActor.h |
| **RELAY** | Group 2 Solenoid | ❌ Not Done | Low | bool | StateActor.h |
| **RELAY** | Tea Water Solenoid | ❌ Not Done | Low | bool | StateActor.h |
| **DIAG** | Device Name | ❌ Not Done | Low | String | StaticConfig.h |
| **DIAG** | Firmware Version | ❌ Not Done | Low | String | platformio.ini |
| **DIAG** | Machine Type | ❌ Not Done | Low | String | UserConfig.h |
| **DIAG** | Device Uptime | ❌ Not Done | Low | uint32 | main.cpp |

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP32 Dual Core System                  │
├──────────────────────────┬──────────────────────────────────┤
│        Core 0            │          Core 1                  │
│     (Main Control)       │      (BLE Handler)               │
├──────────────────────────┼──────────────────────────────────┤
│ • InputHandler           │ • BLECoreManager                 │
│ • StateHandler           │ • BLEService                     │
│   - BoilerStateHandler   │ • NimBLE Stack                   │
│   - GroupHeadHandler     │                                  │
│   - TeaStateHandler      │                                  │
│ • StateActor             │ ← Notifies State Changes         │
│ • VolumetricsHelper      │                                  │
├──────────────────────────┼──────────────────────────────────┤
│ UPDATE INTERVAL: 50ms    │ UPDATE INTERVAL: 100ms           │
└──────────────────────────┴──────────────────────────────────┘
                              ↓
                    ┌─────────────────────────┐
                    │   Shared Memory/State   │
                    ├─────────────────────────┤
                    │ • State struct          │
                    │ • VolumetricSettings    │
                    │ • PreferenceHelper      │
                    │ (Thread-safe access)    │
                    └─────────────────────────┘
                              ↓
                    ┌─────────────────────────┐
                    │   BLE Advertisements    │
                    ├─────────────────────────┤
                    │ Service: 12345678...0   │
                    │ • 20+ Characteristics   │
                    │ • Notify + Read + Write │
                    └─────────────────────────┘
                              ↓
                    ┌─────────────────────────┐
                    │   Mobile App / Device   │
                    ├─────────────────────────┤
                    │ • Live Status           │
                    │ • Remote Configuration  │
                    │ • Progress Monitoring   │
                    └─────────────────────────┘
```

---

## Data Flow: Setting Update via BLE

```
BLE Client                  Core 1 (BLE)              Core 0 (Main)           Preferences
   │                            │                         │                        │
   ├─ Write Dose Value ────────→│                         │                        │
   │                            │                         │                        │
   │                        Characteristic                │                        │
   │                        Write Callback                │                        │
   │                            │                         │                        │
   │                            ├─ Update Preference ────→│                        │
   │                            │   PreferenceHelper      │                        │
   │                            │                         │                        │
   │                            │                    Update RAM                    │
   │                            │                    VolumetricSettings            │
   │                            │                         │                        │
   │                            │         ┌───────────────┤                        │
   │                            │         │               ├─ setULong() ──────────→│
   │                            │         │               │                        │
   │ ←─ ACK + Notify ───────────┤         │               │ ←─ Stored to NVRAM ──┤
   │  (new value)               │         │               │                        │
   │                            │         └───────────────┤                        │
```

---

## Preference Keys Registry

### Current (Already in code)
```cpp
enum class PreferenceKey {
    FlowMeterPulsesLeftSingleEspresso      // "FM_P_L_S"
    FlowMeterPulsesRightSingleEspresso     // "FM_P_R_S"
    FlowMeterPulsesLeftDoubleEspresso      // "FM_P_L_D"
    FlowMeterPulsesRightDoubleEspresso     // "FM_P_R_D"
    TeaWaterMilliseconds                   // "T_M"
    BackflushActivationTimeMs              // "BF_ACT_MS" (NEW)
    BackflushDeactivationTimeMs            // "BF_DEACT_MS" (NEW)
}
```

### Proposed Additions
```cpp
    // Auto Backflush
    AutoBackflushExtractDurationMs         // "AB_EXT_MS"
    AutoBackflushPauseDurationMs           // "AB_PAUSE_MS"
    AutoBackflushCycleCount                // "AB_CYCLES"
    
    // Future expansion
    BoilerFillThreshold                    // "BOILER_THRESH"
    BoilerCheckIntervalMs                  // "BOILER_CHK_MS"
    ProgrammingModeTimeoutMs               // "PROG_TIMEOUT_MS"
```

---

## Next Steps Implementation Roadmap

### Phase 1: Complete ✅
- [x] Add NimBLE library
- [x] Create BLEService base class  
- [x] Implement state notification
- [x] Implement backflush settings
- [x] Run BLE on core 1

### Phase 2: High Impact (Recommended Next)
**Estimated: 2-3 hours**

```cpp
// 1. Update PreferenceHelper.h
Add PreferenceKeys:
- AutoBackflushExtractDurationMs
- AutoBackflushPauseDurationMs  
- AutoBackflushCycleCount

// 2. Update GroupHeadHandler.h
Replace hard-coded constants with preference reads

// 3. Update BLEService.h
Add volumetric characteristics (read/write)
- LEFT_SINGLE_ESPRESSO_UUID
- LEFT_DOUBLE_ESPRESSO_UUID
- RIGHT_SINGLE_ESPRESSO_UUID
- RIGHT_DOUBLE_ESPRESSO_UUID
- TEA_WATER_DURATION_UUID

// 4. Update VolumetricsHelper
Add setter methods that work with remote updates
Update getters to read from preferences on demand

// 5. Update BLECoreManager
Add characteristic updates for volumetric values
```

### Phase 3: Better UX (Medium Impact)
**Estimated: 2-3 hours**

```cpp
// Add real-time progress monitoring
- Group 1 flow meter progress (notify on each pulse)
- Group 2 flow meter progress (notify on each pulse)
- Boiler state + probe reading (notify every 2s)

// Add control features
- Remote programming mode toggle
- Relay state reading
```

### Phase 4: Polish (Low Priority)
**Estimated: 1-2 hours**

```cpp
// Diagnostic characteristics
- Device info (name, version, machine type)
- Hardware status
- Settings verification

// Settings management
- Settings export (all values in one read)
- Settings import (bulk write)
- Factory reset command
```

---

## Mobile App Integration Example

```javascript
// Pseudo-code for mobile app using BLE characteristics

// Read current settings
async function loadSettings() {
    const leftSingle = await readCharacteristic(LEFT_SINGLE_UUID);
    const rightDouble = await readCharacteristic(RIGHT_DOUBLE_UUID);
    const teaDuration = await readCharacteristic(TEA_WATER_UUID);
    // Display in UI
}

// Update a dose setting
async function updateDose(position, amount) {
    await writeCharacteristic(position, amount);
    // Confirm by reading back
}

// Monitor extraction in real-time
function subscribeToProgress() {
    subscribeToNotifications(GROUP_ONE_PROGRESS_UUID, (pulses, target) => {
        updateProgressBar(pulses / target);
    });
}

// Watch boiler status
function subscribeToBoiler() {
    subscribeToNotifications(BOILER_STATE_UUID, (filling, probeValue) => {
        updateBoilerDisplay(filling, probeValue);
    });
}
```
