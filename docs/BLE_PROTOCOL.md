# BrewPilot BLE Protocol

## Service

**UUID:** `12345678-1234-5678-1234-56789abcdef0`

## Characteristics

### State Characteristics (Read + Notify)

| Name               | UUID      | Type   | Description                                                                                                                  |
| ------------------ | --------- | ------ | ---------------------------------------------------------------------------------------------------------------------------- |
| State              | `...def1` | Struct | Main state (5 bytes): isFillingBoiler, groupOneIsExtracting, groupTwoIsExtracting, isExtractingTeaWater, isInProgrammingMode |
| Boiler State       | `...def2` | Struct | Boiler info (5 bytes): isFillingBoiler, boilerProbeRawValue (uint16), boilerState (uint8)                                    |
| Group One Progress | `...def3` | Struct | Progress (8 bytes): currentPulses (uint32), targetPulses (uint32)                                                            |
| Group Two Progress | `...def4` | Struct | Progress (8 bytes): currentPulses (uint32), targetPulses (uint32)                                                            |

### Backflush Settings (Read + Write)

| Name                | UUID      | Type   | Description             |
| ------------------- | --------- | ------ | ----------------------- |
| Group One Backflush | `...def5` | uint16 | Activation time in ms   |
| Group Two Backflush | `...def6` | uint16 | Deactivation time in ms |

### Volumetric Settings (Read Only)

| Name                  | UUID       | Type   | Description       |
| --------------------- | ---------- | ------ | ----------------- |
| Left Single Espresso  | `...def7`  | uint32 | Flow meter pulses |
| Left Double Espresso  | `...def8`  | uint32 | Flow meter pulses |
| Right Single Espresso | `...def9`  | uint32 | Flow meter pulses |
| Right Double Espresso | `...def10` | uint32 | Flow meter pulses |
| Tea Water Duration    | `...def11` | uint32 | Duration setting  |

### Auto Backflush Settings (Read + Write)

| Name             | UUID       | Type   | Description            |
| ---------------- | ---------- | ------ | ---------------------- |
| Extract Duration | `...def12` | uint16 | Extract duration in ms |
| Pause Duration   | `...def13` | uint16 | Pause duration in ms   |
| Cycles           | `...def14` | uint16 | Number of cycles       |

### Device Info (Read Only)

| Name             | UUID       | Type   | Description       |
| ---------------- | ---------- | ------ | ----------------- |
| Device Name      | `...def15` | String | Device identifier |
| Firmware Version | `...def16` | String | Version string    |
| Machine Type     | `...def17` | String | Machine model     |

### Control (Read + Write)

| Name             | UUID       | Type  | Description           |
| ---------------- | ---------- | ----- | --------------------- |
| Programming Mode | `...def18` | uint8 | 0=disabled, 1=enabled |

## Data Formats

### State Structure

```c
struct {
    bool isFillingBoiler;
    bool groupOneIsExtracting;
    bool groupTwoIsExtracting;
    bool isExtractingTeaWater;
    bool isInProgrammingMode;
} // 5 bytes packed
```

### Boiler State Structure

```c
struct {
    bool isFillingBoiler;
    uint16_t boilerProbeRawValue;
    uint8_t boilerState; // 0=above_filled, 1=above_filling, 2=below_target
} // 5 bytes packed
```

### Progress Structure

```c
struct {
    uint32_t currentPulses;
    uint32_t targetPulses;
} // 8 bytes
```

## Byte Order

All multi-byte values are little-endian.

## Update Intervals

- State updates: On change
- Settings refresh: Every 5 seconds
- Boiler state: On change
- Progress: On change during extraction
