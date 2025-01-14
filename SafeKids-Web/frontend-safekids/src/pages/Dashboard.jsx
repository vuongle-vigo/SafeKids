import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "../components/ui/card";
import { useState, useEffect } from "react";
import axiosInstance from "../api/axios";
import { useNavigate, Link } from "react-router-dom";

export default function Dashboard() {
  const [devices, setDevices] = useState([]); // Danh sách thiết bị từ backend
  const [onlineDevices, setOnlineDevices] = useState([]); // Danh sách các thiết bị online
  const navigate = useNavigate();

  useEffect(() => {
    // Fetch danh sách thiết bị từ backend
    const fetchDevices = async () => {
      try {
        const token = localStorage.getItem("token");
        if (!token) {
          navigate("/login");
          return;
        }

        const response = await axiosInstance.get("/api/devices");
        setDevices(response.data);
      } catch (error) {
        console.error("Failed to fetch devices:", error);
      }
    };

    const fetchOnlineDevices = async () => {
      try {
        const response = await axiosInstance.get("/api/commands/getClientsOnline");
        console.log(response.data);
        setOnlineDevices(response.data.clients);
      } catch (error) {
        console.error("Failed to fetch online devices:", error);
      }
    };

    fetchDevices();
    fetchOnlineDevices();

    // Polling mỗi 5 giây để cập nhật danh sách thiết bị online
    const interval = setInterval(fetchOnlineDevices, 5000);

    return () => clearInterval(interval); // Cleanup interval khi component unmount
  }, [navigate]);

  // Kiểm tra thiết bị có online hay không
  const isDeviceOnline = (deviceId) => onlineDevices.includes(deviceId);

  return (
    <div className="flex-1 p-4">
      <Card>
        <CardHeader>
          <CardTitle>Devices</CardTitle>
          <CardDescription>
            Here is a list of all your registered devices.
          </CardDescription>
        </CardHeader>
        <CardContent>
          {devices.length > 0 ? (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
              {devices.map((device) => (
                <Link
                  to={`/device/${device.device_id}`}
                  key={device.device_id}
                  className="block"
                >
                  <Card className="border p-4 shadow-md rounded-lg hover:shadow-lg transition-shadow duration-300">
                    <CardHeader>
                      <CardTitle>{device.device_name}</CardTitle>
                      <CardDescription className="text-sm text-gray-500">
                        Device ID: {device.device_id}
                      </CardDescription>
                    </CardHeader>
                    <CardContent>
                      <p>
                        <strong>User ID:</strong> {device.user_id}
                      </p>
                      <p>
                        <strong>Created At:</strong>{" "}
                        {new Date(device.created_at).toLocaleString()}
                      </p>
                      <p>
                        <strong>Last Activity:</strong>{" "}
                        {new Date(device.last_activity).toLocaleString()}
                      </p>
                      <p>
                        <strong>Status:</strong>{" "}
                        <span
                          className={`font-semibold ${
                            isDeviceOnline(device.device_id)
                              ? "text-green-500"
                              : "text-gray-500"
                          }`}
                        >
                          {isDeviceOnline(device.device_id)
                            ? "Online"
                            : "Offline"}
                        </span>
                      </p>
                    </CardContent>
                  </Card>
                </Link>
              ))}
            </div>
          ) : (
            <p>No devices found.</p>
          )}
        </CardContent>
      </Card>
    </div>
  );
}
