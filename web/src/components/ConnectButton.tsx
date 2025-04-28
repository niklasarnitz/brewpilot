import { useBluetooth } from "../hooks/useBluetooth";

interface ConnectButtonProps {
  className?: string;
}

export default function ConnectButton({ className }: ConnectButtonProps) {
  const { isSupported, isConnected, connecting, connect, disconnect } =
    useBluetooth();

  const handleClick = async () => {
    if (isConnected) {
      disconnect();
    } else {
      await connect();
    }
  };

  if (!isSupported) {
    return (
      <button
        disabled
        className={`px-4 py-2 rounded-md bg-gray-700 text-gray-400 cursor-not-allowed transition-colors ${className}`}
      >
        Bluetooth Not Supported
      </button>
    );
  }

  return (
    <button
      onClick={handleClick}
      disabled={connecting}
      className={`px-4 py-2 rounded-md font-medium transition-all focus:outline-none focus:ring-2 focus:ring-offset-2 ${
        isConnected
          ? "bg-red-600 hover:bg-red-700 text-white focus:ring-red-500"
          : "bg-primary-light hover:bg-primary text-white focus:ring-primary"
      } ${connecting ? "opacity-70 cursor-wait" : ""} ${className}`}
    >
      {connecting
        ? "Connecting..."
        : isConnected
        ? "Disconnect"
        : "Connect to Machine"}
    </button>
  );
}
