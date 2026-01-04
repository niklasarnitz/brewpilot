# BrewPilot BLE Enhancement Analysis

## Overview
A comprehensive analysis of all features and settings that could be exposed via BLE in the BrewPilot machine control system.

---

## ✅ Currently Implemented (in feature/ble-interface)

### State Broadcasting
- Machine state (boiler, group heads, tea extraction, programming mode)
- Real-time notifications when state changes

### Settings (Read/Write via BLE)
- Backflush activation time (milliseconds)
- Backflush deactivation time (milliseconds)

---

## 📊 Volumetric Settings (High Priority)

All espresso dose settings should be remotely configurable:

| Setting | Current Location | Data Type | Range | Notes |
|---------|------------------|-----------|-------|-------|
| Left Single Espresso | VolumetricsHelper | uint32 | 0-4,294,967,295 pulses | Read via `getFlowMeterSetting()` |
| Left Double Espresso | VolumetricsHelper | uint32 | 0-4,294,967,295 pulses | Read via `getFlowMeterSetting()` |
| Right Single Espresso | VolumetricsHelper | uint32 | 0-4,294,967,295 pulses | Read via `getFlowMeterSetting()` |
| Right Double Espresso | VolumetricsHelper | uint32 | 0-4,294,967,295 pulses | Read via `getFlowMeterSetting()` |
| Tea Water Duration | VolumetricsHelper | uint32 | 0-4,294,967,295 ms | Currently only accessible via physical buttons |

**BLE Benefit**: Remote configuration without physical access to machine or manual programming mode.

---

## ⚙️ Auto Backflush Configuration (Medium Priority)

Currently hard-coded in `GroupHeadHandler.h`:

```cpp
static const int EXTRACT_DURATION_MS = 10000;   // Extract phase
static const int PAUSE_DURATION_MS = 5000;      // Pause between cycles
static const int TOTAL_CYCLES = 5;              // Number of cycles
```

**Proposal**: Move to preferences and expose via BLE

| Setting | Current Value | Type | Notes |
|---------|---------------|------|-------|
| Auto Backflush Extract Duration | 10,000 ms | uint16 | Duration to run solenoid open |
| Auto Backflush Pause Duration | 5,000 ms | uint16 | Duration between cycles |
| Auto Backflush Cycle Count | 5 | uint8 | Number of extract-pause cycles |

**BLE Benefit**: Adjust backflush intensity and duration for different group head conditions.

---

## 🔍 Real-time Monitoring (Medium Priority)

### Flow Meter Progress Data
During espresso extraction, expose real-time progress:

| Data | Type | Update Frequency | Use Case |
|------|------|------------------|----------|
| Group 1 Current Pulses | uint32 | On every pulse | Monitor extraction progress |
| Group 1 Target Pulses | uint32 | On change | Display expected vs actual |
| Group 2 Current Pulses | uint32 | On every pulse | Monitor extraction progress |
| Group 2 Target Pulses | uint32 | On change | Display expected vs actual |

**BLE Benefit**: Mobile app can show live extraction progress with visual feedback.

### Boiler Monitoring
From `BoilerStateHandler.h`:

| Data | Type | Current Update | Proposed BLE |
|------|------|-----------------|--------------|
| Is Filling Boiler | bool | Event-driven | Real-time notification |
| Boiler Probe Raw Value | uint16 | Every 2 seconds | Real-time notification |
| Boiler State | enum | Every 2 seconds | Real-time notification |
| Last Fill Time | uint32 | Calculated | Periodic update |

**BLE Benefit**: Monitor boiler status, detect sensor issues, optimize heating.

---

## 🎮 Control Features (Medium Priority)

### Programming Mode Control (Remote)
Currently only via physical button combination (CONTINUOUS_HELD).

**Proposal**: Allow enabling/disabling programming mode via BLE characteristic

| Feature | Current | BLE Enhancement |
|---------|---------|-----------------|
| Enter Programming Mode | Physical button | Physical + BLE command |
| Exit Programming Mode | Physical button | Physical + BLE command |
| Verify Settings | Machine display | BLE read characteristics |
| Update Doses | Machine display | Machine display + BLE |

**BLE Benefit**: Remote programming without physical access.

---

## 📋 Diagnostic Information (Low Priority)

Read-only characteristics for diagnostics:

| Info | Type | Notes |
|------|------|-------|
| Device Name | String | "BrewPilot" or machine-specific |
| Firmware Version | String | Current build version |
| Machine Type | String/Enum | "Rancilio S27", "LaCimbali M29", etc |
| Hardware Model | String | Board info |
| Uptime | uint32 | Seconds since boot |
| Last Settings Save | uint32 | Timestamp |

**BLE Benefit**: Remote diagnostics, version checking, machine identification.

---

## 🔌 Relay/Solenoid State Monitoring (Low Priority)

From `StateActor.h`, expose relay states:

| Relay | Current State | BLE Read |
|-------|---------------|----------|
| Pump Relay | Derived from extraction states | Read characteristic |
| Boiler Fill Solenoid | Direct state variable | Read characteristic |
| Group 1 Solenoid | Direct state variable | Read characteristic |
| Group 2 Solenoid | Direct state variable | Read characteristic |
| Tea Water Solenoid | Direct state variable | Read characteristic |

**BLE Benefit**: Verify relay operation, troubleshoot hardware issues.

---

## 📱 Recommended BLE Characteristic Structure

### Service UUID
`12345678-1234-5678-1234-56789abcdef0`

### State & Monitoring Characteristics (Read + Notify)
- `56789abcdef1`: Combined machine state
- `56789abcdef2`: Boiler state + probe reading
- `56789abcdef3`: Group 1 extraction progress
- `56789abcdef4`: Group 2 extraction progress

### Settings Characteristics (Read + Write)
**Backflush** (already implemented)
- Activation time
- Deactivation time

**Volumetrics** (high priority)
- Left single espresso pulses
- Left double espresso pulses
- Right single espresso pulses
- Right double espresso pulses
- Tea water duration

**Auto Backflush** (medium priority)
- Extract duration
- Pause duration
- Cycle count

**Control** (medium priority)
- Programming mode enable/disable

### Diagnostic Characteristics (Read-only)
- Device name
- Firmware version
- Machine type
- Machine configuration

---

## 🛠️ Implementation Tasks (Priority Order)

### Phase 1 (Already Done ✅)
1. ✅ Add NimBLE library to platformio.ini
2. ✅ Create BLEService base class
3. ✅ Implement state broadcasting
4. ✅ Implement backflush settings (read/write)
5. ✅ Run BLE on core 1 (second processor)

### Phase 2 (High Priority)
1. Extend PreferenceHelper for auto-backflush settings
2. Move auto-backflush constants to preferences
3. Add volumetric settings characteristics (read/write)
4. Update VolumetricsHelper to accept remote updates

### Phase 3 (Medium Priority)
1. Add real-time progress characteristics for flow meters
2. Add boiler monitoring characteristics
3. Implement remote programming mode control
4. Add relay state monitoring

### Phase 4 (Low Priority)
1. Add diagnostic characteristics
2. Add device info characteristics
3. Implement settings verification
4. Add telemetry/logging via BLE

---

## 🔄 Data Structure Considerations

### Packing Binary Data
For efficiency, all BLE characteristics use packed structs:

```cpp
struct BLEStateData {
    bool isFillingBoiler;
    bool groupOneIsExtracting;
    bool groupTwoIsExtracting;
    bool isExtractingTeaWater;
    bool isInProgrammingMode;
} __attribute__((packed));
```

### Byte Ordering
- Use little-endian for multi-byte values (standard for Bluetooth)
- Example: uint16_t = [byte0 LSB, byte1 MSB]

### Variable Updates
- Use notification characteristics for frequently changing data
- Update only when value changes (avoid flooding)
- 100ms update interval for core BLE loop is appropriate

---

## 🔐 Security Considerations

- Consider adding BLE pairing/bonding for security
- Implement write validation (range checks, type validation)
- Log all remote setting changes
- Prevent conflicting operations (e.g., remote + physical button simultaneously)

---

## 📱 Example Mobile App Use Cases

1. **Remote Espresso Tuning**: Adjust dose amounts while testing different beans
2. **Backflush Intensity**: Change backflush settings without opening machine
3. **Live Extraction Monitor**: Watch extraction progress in real-time
4. **Boiler Status**: Monitor boiler temperature without local display
5. **Quick Diagnostics**: Verify machine is functioning correctly remotely
6. **Settings Backup/Restore**: Export/import machine settings

---

## Summary

**Current Implementation**: ✅ 2 features (state + backflush timing)

**Recommended Complete Implementation**: 25+ features across 4 phases

**Highest ROI Features**:
1. Volumetric settings (dose configuration) - enables remote tuning
2. Auto-backflush settings - enables maintenance customization
3. Flow meter progress - enables mobile app display
4. Programming mode control - enables remote programming

**Total Estimated Characteristics**: 20-25 (structured as 1 service with multiple characteristics)
