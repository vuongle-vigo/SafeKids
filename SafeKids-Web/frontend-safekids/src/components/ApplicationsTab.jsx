export default function ApplicationsTab({ installedApps, restrictedApps, onToggle, onSetLimit, onUninstall }) {
  return (
    <div>
      <h2 className="text-lg font-semibold mb-4">Restricted Applications</h2>
      <ul className="list-disc pl-5">
        {installedApps.map((app) => (
          <li key={app} className="flex flex-col mb-4">
            <div className="flex justify-between items-center">
              <span>{app}</span>
              <button
                onClick={() => onToggle(app)}
                className={`text-sm ${
                  restrictedApps.includes(app)
                    ? "text-red-500 hover:text-red-700"
                    : "text-blue-500 hover:text-blue-700"
                }`}
              >
                {restrictedApps.includes(app) ? "Unrestrict" : "Restrict"}
              </button>
            </div>
            <div className="flex justify-between items-center mt-2">
              <button
                onClick={() => {
                  const limit = prompt(`Set usage limit for ${app} (in minutes):`);
                  if (limit) onSetLimit(app, parseInt(limit, 10));
                }}
                className="text-sm text-green-500 hover:text-green-700"
              >
                Set Time Limit
              </button>
              <button
                onClick={() => {
                  if (window.confirm(`Are you sure you want to uninstall ${app}?`)) {
                    onUninstall(app);
                  }
                }}
                className="text-sm text-gray-500 hover:text-gray-700"
              >
                Uninstall
              </button>
            </div>
          </li>
        ))}
      </ul>
    </div>
  );
}
