import React from "react";
import ReactDOM from "react-dom/client";
import { RouterProvider } from "react-router-dom";
import router from "./routes";
import "./index.css";

// Register service worker for PWA functionality only in production
if ("serviceWorker" in navigator) {
  if (!import.meta.env.DEV) {
    // Production mode: Register service worker
    window.addEventListener("load", () => {
      navigator.serviceWorker
        .register("/sw.js")
        .then((registration) => {
          console.log(
            "Service Worker registered with scope:",
            registration.scope
          );
        })
        .catch((error) => {
          console.error("Service Worker registration failed:", error);
        });
    });
  } else {
    // Development mode: Unregister any existing service workers
    window.addEventListener("load", async () => {
      const registrations = await navigator.serviceWorker.getRegistrations();
      if (registrations.length > 0) {
        for (const registration of registrations) {
          registration.unregister().then(() => {
            console.log("Service Worker unregistered for development mode");
          });
        }
      }
      console.log("PWA functionality disabled in development mode");
    });
  }
}

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <RouterProvider router={router} />
  </React.StrictMode>
);
