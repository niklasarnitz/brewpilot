import { useBluetooth } from "../hooks/useBluetooth";
import { TrashIcon } from "@heroicons/react/24/outline";

export default function EventLog() {
  const { isConnected, eventLogs, clearEventLogs } = useBluetooth();

  if (!isConnected) {
    return (
      <div className="flex h-full flex-col items-center justify-center p-4">
        <div className="text-center">
          <h1 className="text-2xl font-bold text-white">Not Connected</h1>
          <p className="mt-2 text-gray-400">
            Connect to your espresso machine using the button in the sidebar to
            view event logs.
          </p>
        </div>
      </div>
    );
  }

  const formatTimestamp = (timestamp: number) => {
    const date = new Date(timestamp);
    return date.toLocaleTimeString([], {
      hour: "2-digit",
      minute: "2-digit",
      second: "2-digit",
      hour12: false,
    });
  };

  return (
    <div className="space-y-4">
      <div className="flex items-center justify-between">
        <h1 className="text-2xl font-bold text-white">Event Log</h1>
        <button
          onClick={clearEventLogs}
          className="flex items-center space-x-1 px-3 py-2 rounded-md bg-gray-700 text-gray-300 hover:bg-gray-600 transition-colors"
        >
          <TrashIcon className="h-4 w-4" />
          <span>Clear</span>
        </button>
      </div>

      <div className="bg-surface-dark shadow-card-dark rounded-lg p-6 transition-colors">
        <div className="max-h-[70vh] overflow-y-auto">
          {eventLogs.length === 0 ? (
            <div className="py-10 text-center text-gray-400">
              <p>No event logs yet. They will appear here as they occur.</p>
              <p className="mt-2 text-sm">
                Try pressing buttons on your espresso machine.
              </p>
            </div>
          ) : (
            <table className="w-full">
              <thead className="sticky top-0 bg-surface-dark transition-colors">
                <tr className="border-b border-gray-700 text-left text-sm font-semibold">
                  <th className="py-2 pl-2 pr-6 text-gray-300">Time</th>
                  <th className="py-2 pl-2 text-gray-300">Message</th>
                </tr>
              </thead>
              <tbody>
                {eventLogs.map((log, index) => (
                  <tr
                    key={index}
                    className={`border-b border-gray-700 text-sm ${
                      index === eventLogs.length - 1 ? "bg-blue-900/20" : ""
                    }`}
                  >
                    <td className="whitespace-nowrap py-2 pl-2 pr-6 text-gray-400">
                      {formatTimestamp(log.timestamp)}
                    </td>
                    <td className="py-2 pl-2 text-gray-200">
                      <pre className="font-sans">{log.message}</pre>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>

      <div className="mt-4 flex items-center justify-end space-x-2 text-sm text-gray-400">
        <div className="h-2 w-2 rounded-full bg-green-500"></div>
        <p>Listening for events...</p>
      </div>
    </div>
  );
}
