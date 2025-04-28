import Layout from "@/components/Layout";
import Dashboard from "@/pages/Dashboard";
import EventLog from "@/pages/EventLog";
import NotFound from "@/pages/NotFound";
import Settings from "@/pages/Settings";
import { createBrowserRouter } from "react-router-dom";

const router = createBrowserRouter([
  {
    path: "/",
    element: <Layout />,
    errorElement: <NotFound />,
    children: [
      {
        index: true,
        element: <Dashboard />,
      },
      {
        path: "events",
        element: <EventLog />,
      },
      {
        path: "settings",
        element: <Settings />,
      },
    ],
  },
]);

export default router;
