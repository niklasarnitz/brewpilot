import { useBluetooth } from "../hooks/useBluetooth";
import { useState, useEffect } from "react";

export default function Dashboard() {
  const {
    isConnected,
    machineState,
    volumetricSettings,
    isDemoMode,
    toggleDemoMode,
    toggleFillingBoiler,
    toggleTeaWater,
    toggleGroupOne,
    toggleGroupTwo,
    toggleProgrammingMode,
  } = useBluetooth();
  const [showLeftCup, setShowLeftCup] = useState(false);
  const [showRightCup, setShowRightCup] = useState(false);

  // Handle cup animations
  useEffect(() => {
    if (machineState?.groupOneIsExtracting) {
      setShowLeftCup(true);
    } else {
      // Small delay to remove the cup after extraction ends
      const timer = setTimeout(() => setShowLeftCup(false), 1500);
      return () => clearTimeout(timer);
    }
  }, [machineState?.groupOneIsExtracting]);

  useEffect(() => {
    if (machineState?.groupTwoIsExtracting) {
      setShowRightCup(true);
    } else {
      // Small delay to remove the cup after extraction ends
      const timer = setTimeout(() => setShowRightCup(false), 1500);
      return () => clearTimeout(timer);
    }
  }, [machineState?.groupTwoIsExtracting]);

  if (!isConnected) {
    return (
      <div className="flex h-full flex-col items-center justify-center p-4">
        <div className="text-center">
          <h1 className="text-2xl font-bold text-white">Not Connected</h1>
          <p className="mt-2 text-gray-400">
            Connect to your espresso machine using the button in the sidebar to
            view its status.
          </p>
          <button
            onClick={toggleDemoMode}
            className="mt-6 rounded-full bg-primary px-6 py-2 text-white hover:bg-primary-dark focus:outline-none focus:ring-2 focus:ring-primary focus:ring-offset-2 focus:ring-offset-background-dark transition-colors"
          >
            Enter Demo Mode
          </button>
        </div>
      </div>
    );
  }

  return (
    <div className="space-y-6">
      <div className="flex justify-between items-center">
        <h1 className="text-2xl font-bold text-white">Machine Status</h1>
        {isDemoMode && (
          <div className="flex items-center">
            <span className="text-amber-400 animate-pulse mr-2">●</span>
            <span className="text-gray-300 mr-4">Demo Mode</span>
            <button
              onClick={toggleDemoMode}
              className="rounded-full bg-gray-700 px-4 py-1 text-white text-sm hover:bg-gray-600 focus:outline-none focus:ring-2 focus:ring-primary focus:ring-offset-2 focus:ring-offset-background-dark transition-colors"
            >
              Exit Demo
            </button>
          </div>
        )}
      </div>

      {/* Machine Visualization - Now at the top */}
      <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
        <h2 className="mb-4 text-xl font-semibold text-white">
          Machine Visualization
        </h2>

        <div className="aspect-w-16 aspect-h-9 relative mx-auto h-96 max-w-3xl overflow-hidden rounded-lg bg-gray-800 transition-colors">
          <div className="absolute inset-0 flex items-center justify-center">
            {/* La Cimbali M29-inspired machine visualization */}
            <div className="relative h-64 w-[400px] rounded-lg bg-gray-700 transition-colors">
              {/* Top chrome section */}
              <div className="absolute top-0 left-0 h-6 w-full rounded-t-lg bg-gray-400"></div>

              {/* Logo area */}
              <div className="absolute top-8 left-1/2 h-10 w-32 -translate-x-1/2 rounded-md bg-gray-500 flex items-center justify-center">
                <span className="text-sm font-bold text-white">BrewPilot</span>
              </div>

              {/* Main body */}
              <div className="absolute top-6 left-0 h-[calc(100%-6px)] w-full rounded-b-lg bg-gradient-to-b from-gray-600 to-gray-700"></div>

              {/* Boiler with animation */}
              <div
                className="absolute top-2 left-1/2 h-10 w-80 -translate-x-1/2 rounded-t-lg bg-gray-500 hover:bg-gray-400 transition-colors"
                onClick={() => toggleFillingBoiler()}
                style={{ cursor: "pointer" }}
              >
                <div
                  className={`absolute bottom-0 left-0 h-4 w-full rounded-b-lg ${
                    machineState?.isFillingBoiler
                      ? "bg-blue-400 animate-pulse"
                      : "bg-gray-600"
                  } transition-colors`}
                >
                  {/* Steam indicators */}
                  {machineState?.isFillingBoiler && (
                    <>
                      <div className="absolute -top-4 left-1/4 h-4 w-1 animate-steam-1 bg-white opacity-70"></div>
                      <div className="absolute -top-6 left-2/4 h-6 w-1 animate-steam-2 bg-white opacity-70"></div>
                      <div className="absolute -top-5 left-3/4 h-5 w-1 animate-steam-3 bg-white opacity-70"></div>
                    </>
                  )}
                </div>
              </div>

              {/* Group heads section */}
              <div className="absolute bottom-0 flex w-full justify-around px-8">
                {/* Left group head */}
                <div className="relative">
                  <div className="absolute -top-12 left-1/2 h-12 w-20 -translate-x-1/2 rounded-t-lg bg-gray-500"></div>
                  <div
                    className={`h-28 w-16 rounded-t-lg ${
                      machineState?.groupOneIsExtracting
                        ? "bg-gray-500"
                        : "bg-gray-600"
                    } transition-colors relative hover:bg-gray-500/80`}
                    onClick={() => toggleGroupOne()}
                    style={{ cursor: "pointer" }}
                  >
                    {/* Portafilter */}
                    <div className="absolute bottom-0 left-1/2 h-8 w-24 -translate-x-1/2 rounded-b-lg bg-gray-800"></div>

                    {/* Water flow animation */}
                    {machineState?.groupOneIsExtracting && (
                      <div className="absolute bottom-0 left-1/2 h-16 w-1 -translate-x-1/2 bg-gradient-to-b from-blue-300 to-transparent animate-water-flow"></div>
                    )}

                    {/* Cup animation */}
                    {showLeftCup && (
                      <div className="absolute -bottom-10 left-1/2 h-10 w-8 -translate-x-1/2 rounded-b-lg bg-white/20 animate-bounce-in">
                        <div className="absolute bottom-0 left-0 h-6 w-8 rounded-b-lg border-2 border-white/30"></div>
                        {/* Coffee level */}
                        {machineState?.groupOneIsExtracting && (
                          <div className="absolute bottom-0 left-0 h-4 w-full rounded-b-lg bg-amber-800/80 animate-fill-cup"></div>
                        )}
                      </div>
                    )}
                  </div>
                </div>

                {/* Tea water wand */}
                <div className="relative mx-4">
                  <div
                    className={`h-20 w-6 rounded-t-lg ${
                      machineState?.isExtractingTeaWater
                        ? "bg-red-400"
                        : "bg-gray-600"
                    } transition-colors hover:bg-red-400/60`}
                    onClick={() => toggleTeaWater()}
                    style={{ cursor: "pointer" }}
                  >
                    {/* Water steam animation */}
                    {machineState?.isExtractingTeaWater && (
                      <div className="absolute bottom-0 left-1/2 h-12 w-2 -translate-x-1/2 bg-gradient-to-b from-blue-100 to-transparent animate-water-flow"></div>
                    )}
                  </div>
                </div>

                {/* Right group head */}
                <div className="relative">
                  <div className="absolute -top-12 left-1/2 h-12 w-20 -translate-x-1/2 rounded-t-lg bg-gray-500"></div>
                  <div
                    className={`h-28 w-16 rounded-t-lg ${
                      machineState?.groupTwoIsExtracting
                        ? "bg-gray-500"
                        : "bg-gray-600"
                    } transition-colors relative hover:bg-gray-500/80`}
                    onClick={() => toggleGroupTwo()}
                    style={{ cursor: "pointer" }}
                  >
                    {/* Portafilter */}
                    <div className="absolute bottom-0 left-1/2 h-8 w-24 -translate-x-1/2 rounded-b-lg bg-gray-800"></div>

                    {/* Water flow animation */}
                    {machineState?.groupTwoIsExtracting && (
                      <div className="absolute bottom-0 left-1/2 h-16 w-1 -translate-x-1/2 bg-gradient-to-b from-blue-300 to-transparent animate-water-flow"></div>
                    )}

                    {/* Cup animation */}
                    {showRightCup && (
                      <div className="absolute -bottom-10 left-1/2 h-10 w-8 -translate-x-1/2 rounded-b-lg bg-white/20 animate-bounce-in">
                        <div className="absolute bottom-0 left-0 h-6 w-8 rounded-b-lg border-2 border-white/30"></div>
                        {/* Coffee level */}
                        {machineState?.groupTwoIsExtracting && (
                          <div className="absolute bottom-0 left-0 h-4 w-full rounded-b-lg bg-amber-800/80 animate-fill-cup"></div>
                        )}
                      </div>
                    )}
                  </div>
                </div>
              </div>

              {/* Control panel */}
              <div
                className="absolute bottom-4 left-1/2 h-10 w-40 -translate-x-1/2 rounded-md bg-gray-800 flex items-center justify-center hover:bg-gray-700 transition-colors"
                onClick={() => toggleProgrammingMode()}
                style={{ cursor: "pointer" }}
              >
                {/* Programming mode indicator */}
                {machineState?.isInProgrammingMode && (
                  <div className="absolute right-2 top-2 h-4 w-4 rounded-full bg-yellow-300 animate-ping"></div>
                )}
                <div className="flex space-x-2">
                  <div className="h-6 w-6 rounded-full bg-gray-600"></div>
                  <div className="h-6 w-6 rounded-full bg-gray-600"></div>
                  <div className="h-6 w-6 rounded-full bg-gray-600"></div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

      <div className="grid grid-cols-1 gap-6 md:grid-cols-2">
        {/* Machine State Card */}
        <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
          <h2 className="mb-4 text-xl font-semibold text-white">
            Current State
          </h2>

          {machineState ? (
            <div className="space-y-3">
              <StatusItem
                label="Boiler Filling"
                active={machineState.isFillingBoiler}
                onToggle={toggleFillingBoiler}
              />
              <StatusItem
                label="Tea Water Active"
                active={machineState.isExtractingTeaWater}
                onToggle={toggleTeaWater}
              />
              <StatusItem
                label="Group 1 Extracting"
                active={machineState.groupOneIsExtracting}
                onToggle={toggleGroupOne}
              />
              <StatusItem
                label="Group 2 Extracting"
                active={machineState.groupTwoIsExtracting}
                onToggle={toggleGroupTwo}
              />
              <StatusItem
                label="Programming Mode"
                active={machineState.isInProgrammingMode}
                onToggle={toggleProgrammingMode}
              />
            </div>
          ) : (
            <p className="text-gray-400">Loading machine state...</p>
          )}
        </div>

        {/* Volumetric Settings Card */}
        <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
          <h2 className="mb-4 text-xl font-semibold text-white">
            Volumetric Settings
          </h2>

          {volumetricSettings ? (
            <div className="space-y-3">
              <div className="flex justify-between">
                <span className="text-gray-300">Left Single Espresso:</span>
                <span className="font-medium text-white">
                  {volumetricSettings.leftSingleEspressoPulses} pulses
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-300">Left Double Espresso:</span>
                <span className="font-medium text-white">
                  {volumetricSettings.leftDoubleEspressoPulses} pulses
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-300">Right Single Espresso:</span>
                <span className="font-medium text-white">
                  {volumetricSettings.rightSingleEspressoPulses} pulses
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-300">Right Double Espresso:</span>
                <span className="font-medium text-white">
                  {volumetricSettings.rightDoubleEspressoPulses} pulses
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-300">Tea Water Duration:</span>
                <span className="font-medium text-white">
                  {(volumetricSettings.teaWaterMilliseconds / 1000).toFixed(1)}{" "}
                  seconds
                </span>
              </div>
            </div>
          ) : (
            <p className="text-gray-400">Loading volumetric settings...</p>
          )}
        </div>
      </div>
    </div>
  );
}

interface StatusItemProps {
  readonly label: string;
  readonly active: boolean;
  readonly onToggle?: () => void;
}

function StatusItem({ label, active, onToggle }: StatusItemProps) {
  return (
    <div className="flex items-center justify-between">
      <span className="text-gray-300">{label}</span>
      <div
        onClick={onToggle}
        className={`flex h-6 w-16 items-center rounded-full px-1 transition-colors ${
          active ? "bg-green-900/30 justify-end" : "bg-gray-700 justify-start"
        } ${onToggle ? "cursor-pointer hover:opacity-80" : ""}`}
      >
        <div
          className={`h-4 w-4 rounded-full shadow-md transition-colors ${
            active ? "bg-green-400" : "bg-gray-500"
          }`}
        ></div>
      </div>
    </div>
  );
}
