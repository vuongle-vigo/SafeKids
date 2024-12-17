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
  const [devices, setDevices] = useState([]);
  const navigate = useNavigate();

  useEffect(() => {
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
    fetchDevices();
  }, [navigate]);

  return (
    <div className="flex-1 p-4 w-full">
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
