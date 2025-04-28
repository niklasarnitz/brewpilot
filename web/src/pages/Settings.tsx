import { useBluetooth } from "../hooks/useBluetooth";
import { InformationCircleIcon } from "@heroicons/react/24/outline";

export default function Settings() {
  const { isConnected, machineState, volumetricSettings } = useBluetooth();

  return (
    <div className="space-y-6">
      <h1 className="text-2xl font-bold text-white">Settings</h1>

      <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors space-y-4">
        <div className="flex items-start space-x-3">
          <InformationCircleIcon className="h-6 w-6 text-primary-light flex-shrink-0" />
          <div>
            <h2 className="text-lg font-medium text-white">About BrewPilot</h2>
            <p className="mt-1 text-gray-300">
              BrewPilot is a monitoring application for espresso machines
              equipped with the BrewPilot ESP32 controller. This Progressive Web
              App uses Web Bluetooth to connect directly to your machine.
            </p>
          </div>
        </div>

        <div className="rounded-md bg-blue-900/30 p-4 text-sm text-blue-300 border border-blue-800">
          <p>
            <strong>Important:</strong> Web Bluetooth is only available on
            Chrome, Edge, and Chrome-based browsers. It is not supported on iOS
            Safari.
          </p>
        </div>

        <div className="pt-4">
          <h3 className="text-md font-medium text-white">App Version</h3>
          <p className="text-gray-400">0.1.0</p>
        </div>
      </div>

      {isConnected && (
        <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
          <h2 className="mb-4 text-xl font-semibold text-white">
            Connection Information
          </h2>

          <div className="space-y-3">
            <div className="flex items-center justify-between">
              <span className="text-gray-300">Status</span>
              <span className="flex items-center">
                <span className="mr-2 h-2 w-2 rounded-full bg-green-500"></span>
                <span className="font-medium text-green-400">Connected</span>
              </span>
            </div>

            <div className="flex justify-between">
              <span className="text-gray-300">Machine State</span>
              <span className="font-medium text-white">
                {machineState ? "Available" : "Not Available"}
              </span>
            </div>

            <div className="flex justify-between">
              <span className="text-gray-300">Volume Settings</span>
              <span className="font-medium text-white">
                {volumetricSettings ? "Available" : "Not Available"}
              </span>
            </div>
          </div>
        </div>
      )}

      <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
        <h2 className="mb-4 text-xl font-semibold text-white">Installation</h2>

        <p className="mb-4 text-gray-300">
          You can install this app on your device for easier access. Look for
          the install option in your browser menu.
        </p>

        <div className="space-y-4 rounded-md bg-gray-800 p-4 border border-gray-700">
          <div>
            <h3 className="font-medium text-gray-200">
              Chrome (Android/Desktop)
            </h3>
            <p className="text-sm text-gray-400">
              Click the menu button (three dots) and select "Install BrewPilot"
              or "Add to Home Screen".
            </p>
          </div>

          <div>
            <h3 className="font-medium text-gray-200">Safari (MacOS)</h3>
            <p className="text-sm text-gray-400">
              Click Share, then "Add to Dock".
            </p>
          </div>
        </div>
      </div>

      <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
        <h2 className="mb-4 text-xl font-semibold text-white">Privacy</h2>

        <p className="text-gray-300">
          This app connects directly to your espresso machine via Bluetooth. No
          data is sent to any servers, and all information remains on your
          device.
        </p>
      </div>
    </div>
  );
}
