// BLE UUIDs matching those defined in the ESP32 code
const SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const STATE_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const VOLUMETRIC_CHAR_UUID = "8e22f82c-5dd5-4f7b-9b44-a1d5bcc83357";
const EVENT_LOG_CHAR_UUID = "2c53f395-c07f-4062-9b7c-adedd5763b2c";

// Machine state interface matching the State struct in the ESP32 code
export interface MachineState {
  isFillingBoiler: boolean;
  isExtractingTeaWater: boolean;
  groupOneIsExtracting: boolean;
  groupTwoIsExtracting: boolean;
  isInProgrammingMode: boolean;
}

// Volumetric settings interface matching the VolumetricSettings struct
export interface VolumetricSettings {
  leftSingleEspressoPulses: number;
  leftDoubleEspressoPulses: number;
  rightSingleEspressoPulses: number;
  rightDoubleEspressoPulses: number;
  teaWaterMilliseconds: number;
}

// Event log message
export interface LogEvent {
  timestamp: number; // JavaScript timestamp
  message: string;
}

// Global storage for connection state
let bluetoothDevice: BluetoothDevice | null = null;
let stateCharacteristic: BluetoothRemoteGATTCharacteristic | null = null;
let volumetricCharacteristic: BluetoothRemoteGATTCharacteristic | null = null;
let eventLogCharacteristic: BluetoothRemoteGATTCharacteristic | null = null;

// Type for subscribers to state changes
type StateChangeCallback = (state: MachineState) => void;
type VolumetricChangeCallback = (settings: VolumetricSettings) => void;
type EventLogCallback = (event: LogEvent) => void;
type ConnectionChangeCallback = (connected: boolean) => void;

// Arrays of callbacks
const stateChangeCallbacks: StateChangeCallback[] = [];
const volumetricChangeCallbacks: VolumetricChangeCallback[] = [];
const eventLogCallbacks: EventLogCallback[] = [];
const connectionChangeCallbacks: ConnectionChangeCallback[] = [];

export const BluetoothService = {
  // Check if Web Bluetooth API is supported
  isSupported(): boolean {
    return "bluetooth" in navigator;
  },

  // Check if device is connected
  isConnected(): boolean {
    return bluetoothDevice !== null && bluetoothDevice.gatt?.connected === true;
  },

  // Connect to the BLE device
  async connect(): Promise<boolean> {
    if (!this.isSupported()) {
      throw new Error("Web Bluetooth API is not supported in this browser");
    }

    try {
      // Request the device with the correct service UUID
      bluetoothDevice = await navigator.bluetooth.requestDevice({
        filters: [{ services: [SERVICE_UUID] }],
      });

      // Add event listener for disconnection
      bluetoothDevice.addEventListener(
        "gattserverdisconnected",
        this.onDisconnected.bind(this)
      );

      // Connect to the GATT server
      const server = await bluetoothDevice.gatt?.connect();
      if (!server) {
        throw new Error("Failed to connect to GATT server");
      }

      // Get the BrewPilot service
      const service = await server.getPrimaryService(SERVICE_UUID);

      // Get all required characteristics
      stateCharacteristic = await service.getCharacteristic(STATE_CHAR_UUID);
      volumetricCharacteristic = await service.getCharacteristic(
        VOLUMETRIC_CHAR_UUID
      );
      eventLogCharacteristic = await service.getCharacteristic(
        EVENT_LOG_CHAR_UUID
      );

      // Set up notifications
      await stateCharacteristic.startNotifications();
      stateCharacteristic.addEventListener(
        "characteristicvaluechanged",
        this.handleStateChange.bind(this)
      );

      await volumetricCharacteristic.startNotifications();
      volumetricCharacteristic.addEventListener(
        "characteristicvaluechanged",
        this.handleVolumetricChange.bind(this)
      );

      await eventLogCharacteristic.startNotifications();
      eventLogCharacteristic.addEventListener(
        "characteristicvaluechanged",
        this.handleEventLog.bind(this)
      );

      // Notify all subscribers about connection
      this.notifyConnectionChange(true);

      // Initial reads
      await this.readMachineState();
      await this.readVolumetricSettings();

      return true;
    } catch (error) {
      console.error("Error connecting to BLE device:", error);
      this.disconnect();
      return false;
    }
  },

  // Disconnect from the device
  disconnect(): void {
    if (bluetoothDevice?.gatt?.connected) {
      bluetoothDevice.gatt.disconnect();
    }

    stateCharacteristic = null;
    volumetricCharacteristic = null;
    eventLogCharacteristic = null;
    bluetoothDevice = null;

    this.notifyConnectionChange(false);
  },

  // Handle device disconnection
  onDisconnected(): void {
    stateCharacteristic = null;
    volumetricCharacteristic = null;
    eventLogCharacteristic = null;

    this.notifyConnectionChange(false);
  },

  // Read the current machine state
  async readMachineState(): Promise<MachineState | null> {
    if (!stateCharacteristic) return null;

    try {
      const value = await stateCharacteristic.readValue();
      const decodedState = this.decodeState(value);
      this.notifyStateChange(decodedState);
      return decodedState;
    } catch (error) {
      console.error("Error reading machine state:", error);
      return null;
    }
  },

  // Read the volumetric settings
  async readVolumetricSettings(): Promise<VolumetricSettings | null> {
    if (!volumetricCharacteristic) return null;

    try {
      const value = await volumetricCharacteristic.readValue();
      const decodedSettings = this.decodeVolumetricSettings(value);
      this.notifyVolumetricChange(decodedSettings);
      return decodedSettings;
    } catch (error) {
      console.error("Error reading volumetric settings:", error);
      return null;
    }
  },

  // Decode the state data from the characteristic
  decodeState(value: DataView): MachineState {
    const decoder = new TextDecoder("utf-8");
    const jsonString = decoder.decode(value);
    return JSON.parse(jsonString) as MachineState;
  },

  // Decode the volumetric settings from the characteristic
  decodeVolumetricSettings(value: DataView): VolumetricSettings {
    const decoder = new TextDecoder("utf-8");
    const jsonString = decoder.decode(value);
    return JSON.parse(jsonString) as VolumetricSettings;
  },

  // Handler for state characteristic changes
  handleStateChange(event: Event): void {
    const characteristic = event.target as BluetoothRemoteGATTCharacteristic;
    const value = characteristic.value;
    if (!value) return;

    const state = this.decodeState(value);
    this.notifyStateChange(state);
  },

  // Handler for volumetric settings characteristic changes
  handleVolumetricChange(event: Event): void {
    const characteristic = event.target as BluetoothRemoteGATTCharacteristic;
    const value = characteristic.value;
    if (!value) return;

    const settings = this.decodeVolumetricSettings(value);
    this.notifyVolumetricChange(settings);
  },

  // Handler for event log characteristic changes
  handleEventLog(event: Event): void {
    const characteristic = event.target as BluetoothRemoteGATTCharacteristic;
    const value = characteristic.value;
    if (!value) return;

    const decoder = new TextDecoder("utf-8");
    const message = decoder.decode(value);

    const logEvent: LogEvent = {
      timestamp: Date.now(),
      message,
    };

    this.notifyEventLog(logEvent);
  },

  // Notify all subscribers about state changes
  notifyStateChange(state: MachineState): void {
    for (const callback of stateChangeCallbacks) {
      callback(state);
    }
  },

  // Notify all subscribers about volumetric settings changes
  notifyVolumetricChange(settings: VolumetricSettings): void {
    for (const callback of volumetricChangeCallbacks) {
      callback(settings);
    }
  },

  // Notify all subscribers about log events
  notifyEventLog(event: LogEvent): void {
    for (const callback of eventLogCallbacks) {
      callback(event);
    }
  },

  // Notify all subscribers about connection changes
  notifyConnectionChange(connected: boolean): void {
    for (const callback of connectionChangeCallbacks) {
      callback(connected);
    }
  },

  // Subscribe to state changes
  onStateChange(callback: StateChangeCallback): () => void {
    stateChangeCallbacks.push(callback);
    return () => {
      const index = stateChangeCallbacks.indexOf(callback);
      if (index !== -1) {
        stateChangeCallbacks.splice(index, 1);
      }
    };
  },

  // Subscribe to volumetric settings changes
  onVolumetricChange(callback: VolumetricChangeCallback): () => void {
    volumetricChangeCallbacks.push(callback);
    return () => {
      const index = volumetricChangeCallbacks.indexOf(callback);
      if (index !== -1) {
        volumetricChangeCallbacks.splice(index, 1);
      }
    };
  },

  // Subscribe to event log messages
  onEventLog(callback: EventLogCallback): () => void {
    eventLogCallbacks.push(callback);
    return () => {
      const index = eventLogCallbacks.indexOf(callback);
      if (index !== -1) {
        eventLogCallbacks.splice(index, 1);
      }
    };
  },

  // Subscribe to connection changes
  onConnectionChange(callback: ConnectionChangeCallback): () => void {
    connectionChangeCallbacks.push(callback);
    return () => {
      const index = connectionChangeCallbacks.indexOf(callback);
      if (index !== -1) {
        connectionChangeCallbacks.splice(index, 1);
      }
    };
  },
};
