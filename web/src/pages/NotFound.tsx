import { Link, useRouteError } from "react-router-dom";
import { HomeIcon } from "@heroicons/react/24/outline";

export default function NotFound() {
  const error = useRouteError();
  console.error(error);

  return (
    <div className="flex min-h-screen flex-col items-center justify-center bg-background-dark px-6 py-12 transition-colors">
      <div className="text-center">
        <h1 className="mt-6 text-3xl font-bold tracking-tight text-white">
          Page Not Found
        </h1>
        <p className="mt-3 text-base text-gray-400">
          Sorry, we couldn't find the page you're looking for.
        </p>
        <div className="mt-6">
          <Link
            to="/"
            className="inline-flex items-center rounded-md bg-primary-light hover:bg-primary px-4 py-2 text-sm font-medium text-white transition-colors"
          >
            <HomeIcon className="mr-2 h-5 w-5" />
            Go back home
          </Link>
        </div>
      </div>
    </div>
  );
}
