import { useParams } from "react-router-dom";
import { useState } from "react";
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs";
import { TimeUsageTag, ProcessListTag, ScreenshotTag } from "../components/Tags";

export default function DeviceDetails() {
  const { deviceId } = useParams();
  const [activeTab, setActiveTab] = useState("time-usage");

  // Mock data
  const device = {
    device_name: "DESKTOP-FK89NTS",
    last_activity: "November 28",
  };

  return (
    <div className="flex h-screen bg-gray-50">
      {/* Sidebar - Thông tin thiết bị và Tabs */}
      <div className="w-80 bg-white shadow-md p-4 flex flex-col items-center gap-8">
        {/* Thông tin thiết bị */}
        <div className="text-center">
          <h1 className="text-lg font-bold mb-2">{device.device_name}</h1>
          <p className="text-sm text-gray-500 mb-10">Last activity: {device.last_activity}</p>
        </div>

        {/* Tabs */}
        <Tabs
          defaultValue="time-usage"
          orientation="vertical"
          onValueChange={(value) => setActiveTab(value)}
          className="w-full"
        >
          <TabsList className="flex flex-col space-y-3">
            <TabsTrigger
              value="time-usage"
              className="w-full text-left px-4 py-2 rounded-md font-medium hover:bg-gray-200 data-[state=active]:bg-blue-500 data-[state=active]:text-white"
            >
              Time Usage
            </TabsTrigger>
            <TabsTrigger
              value="process-list"
              className="w-full text-left px-4 py-2 rounded-md font-medium hover:bg-gray-200 data-[state=active]:bg-blue-500 data-[state=active]:text-white"
            >
              Process List
            </TabsTrigger>
            <TabsTrigger
              value="screenshot"
              className="w-full text-left px-4 py-2 rounded-md font-medium hover:bg-gray-200 data-[state=active]:bg-blue-500 data-[state=active]:text-white"
            >
              Screenshot
            </TabsTrigger>
          </TabsList>
        </Tabs>
      </div>

      {/* Nội dung của Tab - Bên phải */}
      <div className="flex-1 p-6 bg-white shadow rounded-lg">
        {activeTab === "time-usage" && <TimeUsageTag deviceId={deviceId} />}
        {activeTab === "process-list" && <ProcessListTag deviceId={deviceId} />}
        {activeTab === "screenshot" && <ScreenshotTag deviceId={deviceId} />}
      </div>
    </div>
  );
}
