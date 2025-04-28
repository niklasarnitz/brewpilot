import { useState, useEffect } from "react";
import {
  BluetoothService,
  MachineState,
  VolumetricSettings,
  LogEvent,
} from "../services/BluetoothService";
import { DemoService } from "../services/DemoService";

export function useBluetooth() {
  const [isConnected, setIsConnected] = useState<boolean>(
    BluetoothService.isConnected() || DemoService.isDemoModeActive()
  );
  const [isSupported, setIsSupported] = useState<boolean>(
    BluetoothService.isSupported()
  );
  const [machineState, setMachineState] = useState<MachineState | null>(null);
  const [volumetricSettings, setVolumetricSettings] =
    useState<VolumetricSettings | null>(null);
  const [eventLogs, setEventLogs] = useState<LogEvent[]>([]);
  const [connecting, setConnecting] = useState<boolean>(false);
  const [isDemoMode, setIsDemoMode] = useState<boolean>(
    DemoService.isDemoModeActive()
  );

  useEffect(() => {
    // Check if Web Bluetooth API is supported
    setIsSupported(BluetoothService.isSupported());

    // Subscribe to connection changes
    const unsubscribeConnection = BluetoothService.onConnectionChange(
      (connected) => {
        setIsConnected(connected || DemoService.isDemoModeActive());
      }
    );

    // Subscribe to state changes
    const unsubscribeState = BluetoothService.onStateChange((state) => {
      setMachineState(state);
    });

    // Subscribe to volumetric settings changes
    const unsubscribeVolumetric = BluetoothService.onVolumetricChange(
      (settings) => {
        setVolumetricSettings(settings);
      }
    );

    // Subscribe to event logs
    const unsubscribeEventLog = BluetoothService.onEventLog((event) => {
      setEventLogs((prev) => [...prev, event].slice(-100)); // Keep the last 100 events
    });

    // Cleanup subscriptions on unmount
    return () => {
      unsubscribeConnection();
      unsubscribeState();
      unsubscribeVolumetric();
      unsubscribeEventLog();
      // If we're in demo mode when unmounting, stop it
      if (DemoService.isDemoModeActive()) {
        DemoService.stopDemoMode();
      }
    };
  }, []);

  // Connect to the device
  const connect = async () => {
    if (!isSupported) return false;

    setConnecting(true);
    try {
      const success = await BluetoothService.connect();
      return success;
    } catch (error) {
      console.error("Error connecting to device:", error);
      return false;
    } finally {
      setConnecting(false);
    }
  };

  // Disconnect from the device
  const disconnect = () => {
    // If in demo mode, stop it
    if (DemoService.isDemoModeActive()) {
      DemoService.stopDemoMode();
      setIsDemoMode(false);
    }
    // Also disconnect from any actual BT connection
    BluetoothService.disconnect();
  };

  // Toggle demo mode
  const toggleDemoMode = () => {
    if (DemoService.isDemoModeActive()) {
      DemoService.stopDemoMode();
      setIsDemoMode(false);
    } else {
      // Disconnect from actual device first if connected
      if (BluetoothService.isConnected()) {
        BluetoothService.disconnect();
      }
      DemoService.startDemoMode();
      setIsDemoMode(true);
    }
  };

  // Clear event logs
  const clearEventLogs = () => {
    setEventLogs([]);
  };

  // Toggle a specific machine state
  const toggleState = (stateName: keyof MachineState, value?: boolean) => {
    // If in demo mode, use the DemoService to toggle state
    if (DemoService.isDemoModeActive()) {
      return DemoService.toggleState(stateName, value);
    }
    // In actual connection mode, use the BluetoothService (if available)
    else if (BluetoothService.isConnected()) {
      // You'd implement actual Bluetooth control here
      console.log(
        `Toggling ${stateName} to ${value ?? "toggle"} via Bluetooth`
      );
      return false; // Placeholder - actual implementation would depend on BluetoothService capabilities
    }
    return false;
  };

  // Convenience functions for specific states
  const toggleFillingBoiler = (value?: boolean) =>
    toggleState("isFillingBoiler", value);
  const toggleTeaWater = (value?: boolean) =>
    toggleState("isExtractingTeaWater", value);
  const toggleGroupOne = (value?: boolean) =>
    toggleState("groupOneIsExtracting", value);
  const toggleGroupTwo = (value?: boolean) =>
    toggleState("groupTwoIsExtracting", value);
  const toggleProgrammingMode = (value?: boolean) =>
    toggleState("isInProgrammingMode", value);

  // Update volumetric settings
  const updateVolumetricSettings = (settings: Partial<VolumetricSettings>) => {
    if (DemoService.isDemoModeActive()) {
      DemoService.updateVolumetricSettings(settings);
    }
    // Actual Bluetooth implementation would go here
  };

  return {
    isSupported,
    isConnected,
    connecting,
    machineState,
    volumetricSettings,
    eventLogs,
    isDemoMode,
    connect,
    disconnect,
    toggleDemoMode,
    clearEventLogs,
    // New functions for controlling machine state
    toggleState,
    toggleFillingBoiler,
    toggleTeaWater,
    toggleGroupOne,
    toggleGroupTwo,
    toggleProgrammingMode,
    updateVolumetricSettings,
  };
}
