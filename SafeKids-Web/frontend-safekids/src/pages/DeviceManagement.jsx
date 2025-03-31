import { useParams } from "react-router-dom";
import { useState, useEffect } from "react";
import ScreenTimeTab from "../components/ScreenTimeTab";
import ApplicationsTab from "../components/ApplicationsTab";
import ScreenshotsTab from "../components/ScreenshotsTab";
import ActivitysTab from "../components/ActivitysTab";
import axios from "axios";

export default function DeviceManagement() {
  const { deviceId } = useParams();
  const [device, setDevice] = useState(null);
  const [activeTab, setActiveTab] = useState("screenTime"); // Default tab
  const [config, setConfig] = useState({ screenTimeLimit: 4, restrictedApps: ["Facebook"] });
  const [screenshots, setScreenshots] = useState([]);
  const [activeActivities, setActiveActivities] = useState(["Google", "YouTube"]);
  const [usageData, setUsageData] = useState([]); // Add state for usage data

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
    const fetchTabData = async () => {
      try {
        switch (activeTab) {
          case "screenTime":
            const token = localStorage.getItem("token");
            if (!token) {
                navigate("/login");
                return;
            }

            const screenTimeResponse = await axios.get(`/api/devices/${deviceId}/screen-time`, {
                headers: {
                Authorization: `Bearer ${token}`,
                },
            });
            console.log("Screen Time Response:", screenTimeResponse.data);
            setConfig((prevConfig) => ({
              ...prevConfig,
              screenTimeLimit: screenTimeResponse.data.limit.daily_limit,
            }));
            setUsageData(screenTimeResponse.data.usage); // Pass usage data to ScreenTimeTab
            break;
          case "activity":
            const activityResponse = await axios.get(`/api/devices/${deviceId}/activities`);
            setActiveActivities(activityResponse.data.activities);
            break;
          case "applications":
            const appsResponse = await axios.get(`/api/devices/${deviceId}/applications`);
            setConfig((prevConfig) => ({ ...prevConfig, restrictedApps: appsResponse.data.restrictedApps }));
            break;
          case "screenshots":
            const screenshotsResponse = await axios.get(`/api/devices/${deviceId}/screenshots`);
            setScreenshots(screenshotsResponse.data.screenshots);
            break;
          default:
            break;
        }
      } catch (error) {
        console.error(`Error fetching data for tab ${activeTab}:`, error);
      }
    };

    if (device) {
      fetchTabData();
    }
  }, [activeTab, device, deviceId]);

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
          />
        );
      case "activity":
        return <ActivitysTab activeActivities={activeActivities} />;
      case "applications":
        return (
          <ApplicationsTab
            installedApps={device.installedApps}
            restrictedApps={config.restrictedApps}
            onToggle={handleAppRestrictionToggle}
          />
        );
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
          <header className="mb-4 flex items-center space-x-4 -mt-6"> {/* Added -mt-2 to lift the header */}
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
                Screen Time
              </button>
              <button
                onClick={() => setActiveTab("activity")}
                className={`pb-2 ${
                  activeTab === "activity"
                    ? "border-b-2 border-blue-500 text-blue-500"
                    : "text-gray-500 hover:text-blue-500"
                }`}
              >
                Activity
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
