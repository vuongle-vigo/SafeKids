import { useParams } from "react-router-dom";
import { useState, useEffect } from "react";
import ScreenTimeTab from "./Tabs/PowerUsageTab";
import ApplicationsTab from "./Tabs/ApplicationsTab";
import ScreenshotsTab from "./Tabs/ScreenshotsTab";
import axios from "axios";
import ApplicationUsageTab from "./Tabs/ApplicationUsageTab";
import WebHistoryTab from "./Tabs/WebHistoryTab";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs";
import { Badge } from "@/components/ui/badge";
import { Skeleton } from "@/components/ui/skeleton";

export default function DeviceManagement() {
  const { deviceId } = useParams();
  const [device, setDevice] = useState(null);
  const [activeTab, setActiveTab] = useState("screenTime");
  const [config, setConfig] = useState({ screenTimeLimit: 4, restrictedApps: ["Facebook"] });
  const [screenshots, setScreenshots] = useState([]);
  const [usageData, setUsageData] = useState([]);
  const [selectedDate, setSelectedDate] = useState(new Date().toISOString().split("T")[0]);

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
            Authorization: `Bearer ${token}`,
          },
        });
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
  }, [activeTab, device]);

  const handleScreenTimeUpdate = (newLimit) => {
    setConfig((prevConfig) => ({ ...prevConfig, screenTimeLimit: newLimit }));
  };

  const handleAppRestrictionToggle = (appName) => {
    const updatedApps = config.restrictedApps.includes(appName)
      ? config.restrictedApps.filter((app) => app !== appName)
      : [...config.restrictedApps, appName];
    setConfig((prevConfig) => ({ ...prevConfig, restrictedApps: updatedApps }));
  };

  return (
    <div className="p-4 space-y-6">
      {device ? (
        <>
          {/* Header */}
          <Card>
            <CardHeader className="flex flex-row items-center space-x-4">
              <CardTitle className="text-xl">{device.device_name}</CardTitle>
              <Badge
                variant={device.device_status === "online" ? "default" : "secondary"}
                className={device.device_status === "online" ? "bg-green-500" : "bg-gray-500"}
              >
                {device.device_status}
              </Badge>
            </CardHeader>
          </Card>

          {/* Tabs */}
          <Tabs value={activeTab} onValueChange={setActiveTab} className="w-full">
            <TabsList className="grid w-full grid-cols-5">
              <TabsTrigger value="screenTime">Thống kê sử dụng máy tính</TabsTrigger>
              <TabsTrigger value="applicationUsage">Thống kê sử dụng ứng dụng</TabsTrigger>
              <TabsTrigger value="applications">Danh sách ứng dụng</TabsTrigger>
              <TabsTrigger value="webHistory">Lịch sử duyệt web</TabsTrigger>
              <TabsTrigger value="screenshots">Chụp ảnh màn hình</TabsTrigger>
            </TabsList>
            <TabsContent value="screenTime">
              <Card>
                <CardContent className="pt-6">
                  <ScreenTimeTab
                    screenTimeLimit={config.screenTimeLimit}
                    usageData={usageData}
                    onUpdate={handleScreenTimeUpdate}
                    deviceId={deviceId}
                  />
                </CardContent>
              </Card>
            </TabsContent>
            <TabsContent value="applicationUsage">
              <Card>
                <CardContent className="pt-6">
                  <ApplicationUsageTab
                    deviceId={deviceId}
                    selectedDate={selectedDate}
                    onDateChange={setSelectedDate}
                    isActive={activeTab === "applicationUsage"}
                  />
                </CardContent>
              </Card>
            </TabsContent>
            <TabsContent value="applications">
              <Card>
                <CardContent className="pt-6">
                  <ApplicationsTab
                    deviceId={deviceId}
                    isActive={activeTab === "applications"}
                  />
                </CardContent>
              </Card>
            </TabsContent>
            <TabsContent value="webHistory">
              <Card>
                <CardContent className="pt-6">
                  <WebHistoryTab deviceId={deviceId} />
                </CardContent>
              </Card>
            </TabsContent>
            <TabsContent value="screenshots">
              <Card>
                <CardContent className="pt-6">
                  <ScreenshotsTab screenshots={screenshots} />
                </CardContent>
              </Card>
            </TabsContent>
          </Tabs>
        </>
      ) : (
        <div className="space-y-4">
          <Skeleton className="h-12 w-full" />
          <Skeleton className="h-8 w-1/3" />
          <Skeleton className="h-64 w-full" />
        </div>
      )}
    </div>
  );
}