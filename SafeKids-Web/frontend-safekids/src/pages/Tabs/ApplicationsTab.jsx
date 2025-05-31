import { useEffect, useState } from "react";
import axios from "axios";
import defaultIcon from '../../resource/default_icon.png'; // Adjust the path as necessary

export default function ApplicationsTab({ isActive, deviceId, restrictedApps, onToggle, onSetLimit, onUninstall }) {
  const [installedApps, setInstalledApps] = useState([]);
  const [searchTerm, setSearchTerm] = useState(""); // State for search term
  const [modalMessage, setModalMessage] = useState(""); // State for modal message
  const [isModalVisible, setIsModalVisible] = useState(false); // State for modal visibility
  const [restrictedState, setRestrictedState] = useState({}); // State to track restriction per app

  useEffect(() => {
    if (!isActive || !deviceId) return; // Fetch data only when the tab is active and deviceId is provided

    const fetchInstalledApps = async () => {
      try {
        const token = localStorage.getItem("token");
        const response = await axios.get(`/api/devices/${deviceId}/installed-apps`, {
          headers: {
            Authorization: `Bearer ${token}`, // Add token to headers
          },
        });
        console.log("Installed apps response:", response.data); // Log the response data
        setInstalledApps(response.data);
      } catch (error) {
        console.error("Error fetching installed apps:", error);
      }
    };

    fetchInstalledApps();
  }, [isActive, deviceId]); // Re-run when `isActive` or `deviceId` changes

  if (!isActive) return null; // Render nothing if the tab is not active

  const filteredApps = installedApps.filter((app) =>
    app.app_name.toLowerCase().includes(searchTerm.toLowerCase())
  ); // Filter apps by search term

  const handleUninstallClick = (app) => {
    if (app.quiet_uninstall_string === null) {
      setModalMessage(`Please manually uninstall ${app.app_name} on the device.`);
      setIsModalVisible(true);
    } else {
      if (window.confirm(`Are you sure you want to uninstall ${app.app_name}? It will be removed on the next device restart.`)) {
        onUninstall(app.app_name);
        setModalMessage(`${app.app_name} will be uninstalled on the next device restart.`);
        setIsModalVisible(true);
      }
    }
  };

  const handleToggleRestriction = (appName) => {
    setRestrictedState((prevState) => ({
      ...prevState,
      [appName]: !prevState[appName], // Toggle the restriction state
    }));
    onToggle(appName); // Call the provided toggle function
  };

  return (
    <div className="p-6 bg-gray-100 rounded-lg shadow-md">
      <div className="flex justify-between items-center mb-4">
        <h2 className="text-xl font-bold">Applications</h2>
        <input
          type="text"
          placeholder="Search apps..."
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          className="px-4 py-2 border rounded-lg shadow-sm focus:outline-none focus:ring focus:border-blue-300"
        />
      </div>
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
        {filteredApps.length > 0 ? (
          filteredApps.map((app) => (
            <div key={app.id} className="bg-white rounded-lg shadow p-4 flex flex-col items-center">
              <div className="w-16 h-16 bg-gray-200 rounded-full flex items-center justify-center mb-4">
                {app.app_icon ? (
                  <img 
                    src={`data:image/png;base64,${app.app_icon}`} 
                    alt={`${app.app_name} icon`} 
                    className="w-full h-full rounded-full" 
                  />
                ) : (
                  <img
                    src={defaultIcon}
                    alt="Default app icon"
                    className="w-full h-full rounded-full"
                  />
                )}
              </div>
              <h3 className="text-lg font-medium text-gray-800 mb-2">{app.app_name || "Unknown App"}</h3>
              <p className="text-sm text-gray-600 mb-1">Version: {app.display_version || "N/A"}</p>
              <p className="text-sm text-gray-600 mb-1">Publisher: {app.publisher || "N/A"}</p>
              <div className="flex flex-col items-center space-y-2">
                <button
                  onClick={() => handleToggleRestriction(app.app_name)}
                  className={`px-3 py-1 rounded text-xs font-medium ${
                    restrictedState[app.app_name]
                      ? "bg-green-500 text-white hover:bg-green-600" // Change to green when unrestricted
                      : "bg-red-500 text-white hover:bg-red-600" // Change to red when restricted
                  }`}
                >
                  {restrictedState[app.app_name] ? "Unrestricted" : "Restricted"}
                </button>
                <button
                  onClick={() => {
                    const limit = prompt(`Set usage limit for ${app.app_name} (in minutes):`);
                    if (limit) onSetLimit(app.app_name, parseInt(limit, 10));
                  }}
                  className="px-3 py-1 rounded bg-green-500 text-white text-xs font-medium hover:bg-green-600"
                >
                  Set Time Limit
                </button>
                <button
                  onClick={() => handleUninstallClick(app)}
                  className="px-3 py-1 rounded bg-gray-500 text-white text-xs font-medium hover:bg-gray-600"
                >
                  Uninstall
                </button>
              </div>
            </div>
          ))
        ) : (
          <p className="text-center text-gray-600">No applications installed on this device.</p>
        )}
      </div>
      {isModalVisible && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center">
          <div className="bg-white p-8 rounded-lg shadow-lg max-w-sm w-full text-center">
            <p className="text-gray-800 mb-6 text-lg font-medium">{modalMessage}</p>
            <button
              onClick={() => setIsModalVisible(false)}
              className="px-6 py-2 bg-blue-500 text-white rounded-lg hover:bg-blue-600"
            >
              Close
            </button>
          </div>
        </div>
      )}
    </div>
  );
}
