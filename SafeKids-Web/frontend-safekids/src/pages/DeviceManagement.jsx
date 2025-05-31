import { useParams } from "react-router-dom";
import { useState, useEffect } from "react";
import ScreenTimeTab from "./Tabs/PowerUsageTab";
import ApplicationsTab from "./Tabs/ApplicationsTab";
import ScreenshotsTab from "./Tabs/ScreenshotsTab";
import axios from "axios";
import ApplicationUsageTab from "./Tabs/ApplicationUsageTab"; // Import the new component
import WebHistoryTab from "./Tabs/WebHistoryTab"; // Import the new component

export default function DeviceManagement() {
  const { deviceId } = useParams();
  const [device, setDevice] = useState(null);
  const [activeTab, setActiveTab] = useState("screenTime"); // Default tab
  const [config, setConfig] = useState({ screenTimeLimit: 4, restrictedApps: ["Facebook"] });
  const [screenshots, setScreenshots] = useState([]);
  const [usageData, setUsageData] = useState([]); // Add state for usage data
  const [selectedDate, setSelectedDate] = useState(new Date().toISOString().split("T")[0]); // State for selected date

  useEffect(() => {
    const fetchDeviceData = async () => {
      try {
        const token = localStorage.getItem("token");
        if (!token) {
          navigate("/login");
          return;
        }

        const response = await axios.get(`/api/devices/${deviceId}`, {
            headers: {
            Authorization: `Bearer ${token}`, // Thêm token vào headers
            },
        });
        // const response = await axios.get(`/api/devices/${deviceId}`);
        setDevice(response.data);
      } catch (error) {
        console.error("Error fetching device data:", error);
      }
    };

    fetchDeviceData();
  }, [deviceId]);

  useEffect(() => {
    if (device) {
      console.log(`Active tab changed to: ${activeTab}`);
    }
  }, [activeTab, device]); // Chỉ log khi tab thay đổi, không gọi API

  const handleScreenTimeUpdate = (newLimit) => {
    setConfig((prevConfig) => ({ ...prevConfig, screenTimeLimit: newLimit }));
  };

  const handleAppRestrictionToggle = (appName) => {
    const updatedApps = config.restrictedApps.includes(appName)
      ? config.restrictedApps.filter((app) => app !== appName)
      : [...config.restrictedApps, appName];
    setConfig((prevConfig) => ({ ...prevConfig, restrictedApps: updatedApps }));
  };

  const renderTabContent = () => {
    switch (activeTab) {
      case "screenTime":
        return (
          <ScreenTimeTab
            screenTimeLimit={config.screenTimeLimit}
            usageData={usageData} // Pass usage data as a prop
            onUpdate={handleScreenTimeUpdate}
            deviceId={deviceId} // Pass deviceId as a prop
          />
        );
      case "applicationUsage": // Updated case for Application Usage
        return (
          <ApplicationUsageTab
            deviceId={deviceId}
            selectedDate={selectedDate}
            onDateChange={setSelectedDate}
            isActive={activeTab === "applicationUsage"} // Truyền isActive
          />
        );
      case "applications":
        return (
          <ApplicationsTab
            deviceId={deviceId} // Only pass deviceId
            isActive={activeTab === "applications"} // Pass isActive prop
          />
        );
      case "webHistory": // New case for Web History
        return <WebHistoryTab deviceId={deviceId} />;
      case "screenshots":
        return <ScreenshotsTab screenshots={screenshots} />;
      default:
        return null;
    }
  };

  return (
    <div className="p-4">
      {device ? (
        <>
          <header className="mb-4 flex items-center space-x-4 -mt-6 pl-8"> {/* Added pl-4 to shift right */}
            <h1 className="text-xl font-semibold">{device.device_name}</h1>
            <span
              className={`text-sm font-medium ${
                device.isOnline ? "text-green-500" : "text-gray-500"
              }`}
            >
              {device.isOnline ? "Online" : "Offline"}
            </span>
          </header>

          {/* Tabs */}
          <div className="mb-4 border-b">
            <nav className="flex space-x-4">
              <button
                onClick={() => setActiveTab("screenTime")}
                className={`pb-2 ${
                  activeTab === "screenTime"
                    ? "border-b-2 border-blue-500 text-blue-500"
                    : "text-gray-500 hover:text-blue-500"
                }`}
              >
                Power Usage
              </button>
              <button
                onClick={() => setActiveTab("applicationUsage")} // Updated tab name
                className={`pb-2 ${
                  activeTab === "applicationUsage"
                    ? "border-b-2 border-blue-500 text-blue-500"
                    : "text-gray-500 hover:text-blue-500"
                }`}
              >
                Application Usage
              </button>
              <button
                onClick={() => setActiveTab("applications")}
                className={`pb-2 ${
                  activeTab === "applications"
                    ? "border-b-2 border-blue-500 text-blue-500"
                    : "text-gray-500 hover:text-blue-500"
                }`}
              >
                Applications
              </button>
              <button
                onClick={() => setActiveTab("webHistory")} // New tab for Web History
                className={`pb-2 ${
                  activeTab === "webHistory"
                    ? "border-b-2 border-blue-500 text-blue-500"
                    : "text-gray-500 hover:text-blue-500"
                }`}
              >
                Web History
              </button>
              <button
                onClick={() => setActiveTab("screenshots")}
                className={`pb-2 ${
                  activeTab === "screenshots"
                    ? "border-b-2 border-blue-500 text-blue-500"
                    : "text-gray-500 hover:text-blue-500"
                }`}
              >
                Screenshots
              </button>
              
            </nav>
          </div>

          {/* Tab Content */}
          <div>{renderTabContent()}</div>
        </>
      ) : (
        <p className="text-gray-500">Loading device data...</p>
      )}
    </div>
  );
}
