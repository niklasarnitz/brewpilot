// filepath: /Users/narnitz/privat/brewpilot/web/src/services/DemoService.ts
import type {
  MachineState,
  VolumetricSettings,
  LogEvent,
} from "./BluetoothService";
import { BluetoothService } from "./BluetoothService";

// Default state for the demo mode
const initialMachineState: MachineState = {
  isFillingBoiler: false,
  isExtractingTeaWater: false,
  groupOneIsExtracting: false,
  groupTwoIsExtracting: false,
  isInProgrammingMode: false,
};

// Default volumetric settings
const defaultVolumetricSettings: VolumetricSettings = {
  leftSingleEspressoPulses: 120,
  leftDoubleEspressoPulses: 240,
  rightSingleEspressoPulses: 120,
  rightDoubleEspressoPulses: 240,
  teaWaterMilliseconds: 5000,
};

// Keep track of the demo mode state
let isDemoMode = false;
let demoIntervalId: number | null = null;
let currentState: MachineState = { ...initialMachineState };
let lastEventTime = 0;
let currentScenario = 0;

// Demo scenarios to cycle through
const demoScenarios = [
  // Boiler filling scenario
  () => {
    currentState = {
      ...currentState,
      isFillingBoiler: true,
      isExtractingTeaWater: false,
      groupOneIsExtracting: false,
      groupTwoIsExtracting: false,
      isInProgrammingMode: false,
    };
    logDemoEvent("Boiler filling started");

    // After 5 seconds, turn off boiler filling
    setTimeout(() => {
      if (!isDemoMode) return;
      currentState = { ...currentState, isFillingBoiler: false };
      BluetoothService.notifyStateChange(currentState);
      logDemoEvent("Boiler filling completed");
    }, 5000);

    return currentState;
  },

  // Left group extraction
  () => {
    currentState = {
      ...currentState,
      isFillingBoiler: false,
      isExtractingTeaWater: false,
      groupOneIsExtracting: true,
      groupTwoIsExtracting: false,
      isInProgrammingMode: false,
    };
    logDemoEvent("Group 1 extraction started");

    // After 25 seconds, turn off left group
    setTimeout(() => {
      if (!isDemoMode) return;
      currentState = { ...currentState, groupOneIsExtracting: false };
      BluetoothService.notifyStateChange(currentState);
      logDemoEvent("Group 1 extraction completed");
    }, 25000);

    return currentState;
  },

  // Right group extraction
  () => {
    currentState = {
      ...currentState,
      isFillingBoiler: false,
      isExtractingTeaWater: false,
      groupOneIsExtracting: false,
      groupTwoIsExtracting: true,
      isInProgrammingMode: false,
    };
    logDemoEvent("Group 2 extraction started");

    // After 25 seconds, turn off right group
    setTimeout(() => {
      if (!isDemoMode) return;
      currentState = { ...currentState, groupTwoIsExtracting: false };
      BluetoothService.notifyStateChange(currentState);
      logDemoEvent("Group 2 extraction completed");
    }, 25000);

    return currentState;
  },

  // Tea water extraction
  () => {
    currentState = {
      ...currentState,
      isFillingBoiler: false,
      isExtractingTeaWater: true,
      groupOneIsExtracting: false,
      groupTwoIsExtracting: false,
      isInProgrammingMode: false,
    };
    logDemoEvent("Tea water extraction started");

    // After 5 seconds, turn off tea water
    setTimeout(() => {
      if (!isDemoMode) return;
      currentState = { ...currentState, isExtractingTeaWater: false };
      BluetoothService.notifyStateChange(currentState);
      logDemoEvent("Tea water extraction completed");
    }, 5000);

    return currentState;
  },

  // Dual group extraction
  () => {
    currentState = {
      ...currentState,
      isFillingBoiler: false,
      isExtractingTeaWater: false,
      groupOneIsExtracting: true,
      groupTwoIsExtracting: true,
      isInProgrammingMode: false,
    };
    logDemoEvent("Dual group extraction started");

    // After 20 seconds, turn off both groups
    setTimeout(() => {
      if (!isDemoMode) return;
      currentState = {
        ...currentState,
        groupOneIsExtracting: false,
        groupTwoIsExtracting: false,
      };
      BluetoothService.notifyStateChange(currentState);
      logDemoEvent("Dual group extraction completed");
    }, 20000);

    return currentState;
  },

  // Programming mode
  () => {
    currentState = {
      ...currentState,
      isFillingBoiler: false,
      isExtractingTeaWater: false,
      groupOneIsExtracting: false,
      groupTwoIsExtracting: false,
      isInProgrammingMode: true,
    };
    logDemoEvent("Programming mode entered");

    // After 10 seconds, exit programming mode
    setTimeout(() => {
      if (!isDemoMode) return;
      currentState = { ...currentState, isInProgrammingMode: false };
      BluetoothService.notifyStateChange(currentState);
      logDemoEvent("Programming mode exited");
    }, 10000);

    return currentState;
  },
];

// Helper function to log demo events
function logDemoEvent(message: string): void {
  const logEvent: LogEvent = {
    timestamp: Date.now(),
    message: `[DEMO] ${message}`,
  };
  BluetoothService.notifyEventLog(logEvent);
}

// Run the next scenario in the demo
function runNextScenario(): void {
  // Check if enough time has passed since the last event (at least 10 seconds)
  const now = Date.now();
  if (now - lastEventTime < 10000) return;

  // Ensure no active extractions are happening before starting a new scenario
  if (
    currentState.groupOneIsExtracting ||
    currentState.groupTwoIsExtracting ||
    currentState.isExtractingTeaWater ||
    currentState.isFillingBoiler ||
    currentState.isInProgrammingMode
  ) {
    return;
  }

  // Run the next scenario
  lastEventTime = now;
  const scenario = demoScenarios[currentScenario];
  const newState = scenario();
  BluetoothService.notifyStateChange(newState);

  // Move to next scenario
  currentScenario = (currentScenario + 1) % demoScenarios.length;
}

export const DemoService = {
  // Start demo mode
  startDemoMode(): void {
    if (isDemoMode) return;

    // Set demo mode flag
    isDemoMode = true;

    // Reset state
    currentState = { ...initialMachineState };

    // Notify of demo connection
    BluetoothService.notifyConnectionChange(true);

    // Send initial state and settings
    BluetoothService.notifyStateChange(currentState);
    BluetoothService.notifyVolumetricChange(defaultVolumetricSettings);

    // Log demo mode start
    logDemoEvent("Demo mode started");

    // Start interval to trigger scenarios
    demoIntervalId = window.setInterval(() => {
      runNextScenario();
    }, 2000); // Check every 2 seconds if we should run a new scenario
  },

  // Stop demo mode
  stopDemoMode(): void {
    if (!isDemoMode) return;

    // Clear the interval
    if (demoIntervalId !== null) {
      window.clearInterval(demoIntervalId);
      demoIntervalId = null;
    }

    // Reset state
    isDemoMode = false;
    currentState = { ...initialMachineState };

    // Log demo mode stop
    logDemoEvent("Demo mode stopped");

    // Notify of disconnection (if not actually connected)
    if (!BluetoothService.isConnected()) {
      BluetoothService.notifyConnectionChange(false);
    }
  },

  // Check if demo mode is active
  isDemoModeActive(): boolean {
    return isDemoMode;
  },

  // Get current state
  getCurrentState(): MachineState | null {
    if (!isDemoMode) return null;
    return { ...currentState };
  },

  // Toggle specific state in demo mode
  toggleState(stateName: keyof MachineState, value?: boolean): boolean {
    if (!isDemoMode) return false;

    // If value is not provided, toggle the current state
    const newValue = value !== undefined ? value : !currentState[stateName];

    // Update the state
    currentState = {
      ...currentState,
      [stateName]: newValue,
    };

    // Log the state change
    logDemoEvent(`${stateName} ${newValue ? "activated" : "deactivated"}`);

    // Notify state change
    BluetoothService.notifyStateChange(currentState);

    // If we're turning something on, automatically turn it off after a delay
    if (newValue === true) {
      const timeouts: Record<string, number> = {
        isFillingBoiler: 5000,
        isExtractingTeaWater: 5000,
        groupOneIsExtracting: 25000,
        groupTwoIsExtracting: 25000,
        isInProgrammingMode: 10000,
      };

      const timeout = timeouts[stateName] || 5000;

      setTimeout(() => {
        if (!isDemoMode) return;
        // Only turn off if it's still on
        if (currentState[stateName]) {
          currentState = { ...currentState, [stateName]: false };
          BluetoothService.notifyStateChange(currentState);
          logDemoEvent(
            `${stateName} automatically deactivated after ${timeout / 1000}s`
          );
        }
      }, timeout);
    }

    return newValue;
  },

  // Update volumetric settings
  updateVolumetricSettings(settings: Partial<VolumetricSettings>): void {
    if (!isDemoMode) return;

    const updatedSettings = {
      ...defaultVolumetricSettings,
      ...settings,
    };

    BluetoothService.notifyVolumetricChange(updatedSettings);
    logDemoEvent("Volumetric settings updated");
  },
};
