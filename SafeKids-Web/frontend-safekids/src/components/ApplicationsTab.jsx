export default function ApplicationsTab({ installedApps, restrictedApps, onToggle, onSetLimit, onUninstall }) {
  return (
    <div className="p-6 bg-gray-100 rounded-lg shadow-md">
      <h2 className="text-2xl font-bold mb-6 text-center text-gray-800">Installed Applications</h2>
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
        {installedApps.map((app) => (
          <div key={app} className="bg-white rounded-lg shadow p-4 flex flex-col items-center">
            <div className="w-16 h-16 bg-gray-200 rounded-full flex items-center justify-center mb-4">
              <span className="text-xl font-semibold text-gray-600">{app[0]}</span>
            </div>
            <h3 className="text-lg font-medium text-gray-800 mb-2">{app}</h3>
            <div className="flex flex-col items-center space-y-2">
              <button
                onClick={() => onToggle(app)}
                className={`px-4 py-2 rounded text-sm font-medium ${
                  restrictedApps.includes(app)
                    ? "bg-red-500 text-white hover:bg-red-600"
                    : "bg-blue-500 text-white hover:bg-blue-600"
                }`}
              >
                {restrictedApps.includes(app) ? "Unrestrict" : "Restrict"}
              </button>
              <button
                onClick={() => {
                  const limit = prompt(`Set usage limit for ${app} (in minutes):`);
                  if (limit) onSetLimit(app, parseInt(limit, 10));
                }}
                className="px-4 py-2 rounded bg-green-500 text-white text-sm font-medium hover:bg-green-600"
              >
                Set Time Limit
              </button>
              <button
                onClick={() => {
                  if (window.confirm(`Are you sure you want to uninstall ${app}?`)) {
                    onUninstall(app);
                  }
                }}
                className="px-4 py-2 rounded bg-gray-500 text-white text-sm font-medium hover:bg-gray-600"
              >
                Uninstall
              </button>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}

// Mock data for testing
const mockInstalledApps = ["YouTube", "Facebook", "Instagram", "TikTok", "Snapchat"];
const mockRestrictedApps = ["YouTube", "TikTok"];

const mockOnToggle = (app) => console.log(`${app} toggled`);
const mockOnSetLimit = (app, limit) => console.log(`Set limit for ${app}: ${limit} minutes`);
const mockOnUninstall = (app) => console.log(`${app} uninstalled`);

export function ApplicationsTabWithMockData() {
  return (
    <ApplicationsTab
      installedApps={mockInstalledApps}
      restrictedApps={mockRestrictedApps}
      onToggle={mockOnToggle}
      onSetLimit={mockOnSetLimit}
      onUninstall={mockOnUninstall}
    />
  );
}
