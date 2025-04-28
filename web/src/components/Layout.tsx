import { useState } from "react";
import { Outlet, NavLink } from "react-router-dom";
import { Bars3Icon, XMarkIcon, BeakerIcon } from "@heroicons/react/24/outline";
import ConnectButton from "@/components/ConnectButton";

export default function Layout() {
  const [sidebarOpen, setSidebarOpen] = useState(false);

  const toggleSidebar = () => {
    setSidebarOpen(!sidebarOpen);
  };

  const closeSidebar = () => {
    setSidebarOpen(false);
  };

  return (
    <div className="min-h-screen bg-background-dark transition-colors duration-200">
      {/* Mobile sidebar overlay */}
      {sidebarOpen && (
        <div
          className="fixed inset-0 z-20 bg-black bg-opacity-50 transition-opacity md:hidden"
          onClick={closeSidebar}
          aria-hidden="true"
        />
      )}

      {/* Sidebar */}
      <div
        className={`fixed inset-y-0 left-0 z-30 w-64 transform bg-surface-dark border-r border-gray-800 shadow-lg transition duration-300 ease-in-out md:translate-x-0 ${
          sidebarOpen ? "translate-x-0" : "-translate-x-full"
        }`}
      >
        <div className="flex h-16 items-center justify-between px-4 border-b border-gray-800">
          <div className="flex items-center">
            <BeakerIcon className="h-8 w-8 text-primary-light" />
            <span className="ml-2 text-xl font-bold text-white">BrewPilot</span>
          </div>
          <button
            className="md:hidden rounded-md p-2 text-gray-400 hover:bg-gray-800 focus:outline-none"
            onClick={closeSidebar}
          >
            <XMarkIcon className="h-6 w-6" />
          </button>
        </div>

        <nav className="mt-5 px-2">
          <NavLink
            to="/"
            className={({ isActive }) =>
              `flex items-center rounded-md px-4 py-2 text-sm font-medium mb-1 transition-colors ${
                isActive
                  ? "bg-primary-dark text-white"
                  : "text-gray-300 hover:bg-gray-800"
              }`
            }
            onClick={closeSidebar}
          >
            Dashboard
          </NavLink>
          <NavLink
            to="/events"
            className={({ isActive }) =>
              `flex items-center rounded-md px-4 py-2 text-sm font-medium mb-1 transition-colors ${
                isActive
                  ? "bg-primary-dark text-white"
                  : "text-gray-300 hover:bg-gray-800"
              }`
            }
            onClick={closeSidebar}
          >
            Event Log
          </NavLink>
          <NavLink
            to="/settings"
            className={({ isActive }) =>
              `flex items-center rounded-md px-4 py-2 text-sm font-medium mb-1 transition-colors ${
                isActive
                  ? "bg-primary-dark text-white"
                  : "text-gray-300 hover:bg-gray-800"
              }`
            }
            onClick={closeSidebar}
          >
            Settings
          </NavLink>
        </nav>

        <div className="absolute bottom-0 left-0 right-0 p-4 border-t border-gray-800">
          <ConnectButton className="w-full" />
        </div>
      </div>

      {/* Main content */}
      <div className="flex flex-col md:pl-64">
        {/* Top header */}
        <header className="sticky top-0 z-10 bg-surface-dark border-b border-gray-800 shadow-sm">
          <div className="flex h-16 items-center justify-between px-4">
            <button
              className="rounded-md p-2 text-gray-400 hover:bg-gray-800 focus:outline-none md:hidden"
              onClick={toggleSidebar}
            >
              <Bars3Icon className="h-6 w-6" />
            </button>
            <div className="md:hidden flex items-center">
              <BeakerIcon className="h-8 w-8 text-primary-light" />
              <span className="ml-2 text-xl font-bold text-white">
                BrewPilot
              </span>
            </div>
          </div>
        </header>

        {/* Main content */}
        <main className="flex-1 p-4">
          <Outlet />
        </main>
      </div>
    </div>
  );
}
