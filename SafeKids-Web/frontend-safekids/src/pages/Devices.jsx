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
import { FaEdit, FaTrash } from "react-icons/fa"; // Import the edit and delete icons

export default function Dashboard() {
  const [devices, setDevices] = useState([]); // Danh sách thiết bị từ backend
  const [onlineDevices, setOnlineDevices] = useState([]); // Danh sách các thiết bị online
  const [editingDeviceId, setEditingDeviceId] = useState(null);
  const [newDeviceName, setNewDeviceName] = useState("");
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
        setOnlineDevices(response.data.clients);
      } catch (error) {
        console.error("Failed to fetch online devices:", error);
      }
    };

    fetchDevices();
    fetchOnlineDevices();

    // Polling mỗi 5 giây để cập nhật danh sách thiết bị online
    const interval = setInterval(fetchOnlineDevices, 500000);

    return () => clearInterval(interval); // Cleanup interval khi component unmount
  }, [navigate]);

  // Kiểm tra thiết bị có online hay không
  const isDeviceOnline = (deviceId) => onlineDevices.includes(deviceId);

  const handleEditClick = (deviceId, currentName) => {
    setEditingDeviceId(deviceId);
    setNewDeviceName(currentName);
  };

  const handleSaveClick = async (deviceId) => {
    if (!newDeviceName.trim()) {
      alert("Device name cannot be empty.");
      return;
    }
    try {
      await axiosInstance.put(`/api/devices/${deviceId}`, { device_name: newDeviceName });
      setDevices((prevDevices) =>
        prevDevices.map((device) =>
          device.device_id === deviceId ? { ...device, device_name: newDeviceName } : device
        )
      );
      setEditingDeviceId(null);
    } catch (error) {
      console.error("Failed to update device name:", error);
      alert("Failed to update device name. Please try again.");
    }
  };

  const handleCancelClick = () => {
    setEditingDeviceId(null);
  };

  const handleDeleteClick = async (deviceId) => {
    if (window.confirm("Are you sure you want to delete this device?")) {
      try {
        await axiosInstance.delete(`/api/devices/${deviceId}`);
        setDevices((prevDevices) =>
          prevDevices.filter((device) => device.device_id !== deviceId)
        );
      } catch (error) {
        console.error("Failed to delete device:", error);
      }
    }
  };

  return (
    <div className="flex-1 p-4">
      <Card>
        <CardHeader>
          <CardTitle>Managed Devices</CardTitle>
          <CardDescription>
            Below is the list of devices you are managing.
          </CardDescription>
        </CardHeader>
        <CardContent>
          {devices.length > 0 ? (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
              {devices.map((device) => (
                <Card
                  key={device.device_id}
                  className="border p-4 shadow-md rounded-lg hover:shadow-lg transition-shadow duration-300"
                >
                  <CardHeader>
                    <CardTitle className="text-lg font-bold flex items-center justify-between">
                      {editingDeviceId === device.device_id ? (
                        <div className="flex items-center space-x-2">
                          <input
                            type="text"
                            value={newDeviceName}
                            onChange={(e) => setNewDeviceName(e.target.value)}
                            className="border rounded px-2 py-1 text-sm"
                            placeholder="Enter new device name"
                          />
                          <button
                            onClick={() => handleSaveClick(device.device_id)}
                            className="text-green-500 hover:text-green-700 text-sm"
                          >
                            Save
                          </button>
                          <button
                            onClick={handleCancelClick}
                            className="text-red-500 hover:text-red-700 text-sm"
                          >
                            Cancel
                          </button>
                        </div>
                      ) : (
                        <span className="truncate max-w-xs" title={device.device_name}>
                          {device.device_name}
                        </span>
                      )}
                      <div className="flex items-center space-x-3">
                        <button
                          onClick={() =>
                            editingDeviceId === device.device_id
                              ? handleSaveClick(device.device_id)
                              : handleEditClick(device.device_id, device.device_name)
                          }
                          className="text-blue-500 hover:text-blue-700"
                          title="Edit Device"
                        >
                          <FaEdit className="w-5 h-5" />
                        </button>
                        <button
                          onClick={() => handleDeleteClick(device.device_id)}
                          className="text-red-500 hover:text-red-700"
                          title="Delete Device"
                        >
                          <FaTrash className="w-5 h-5" />
                        </button>
                      </div>
                    </CardTitle>
                  </CardHeader>
                  <CardContent>
                    <div className="text-sm space-y-2">
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
                          {isDeviceOnline(device.device_id) ? "Online" : "Offline"}
                        </span>
                      </p>
                    </div>
                  </CardContent>
                  <div className="mt-4 flex justify-end">
                    <Link
                      to={`/device/${device.device_id}/manage`}
                      className="text-blue-500 hover:underline text-sm"
                    >
                      View Details
                    </Link>
                  </div>
                </Card>
              ))}
            </div>
          ) : (
            <p className="text-center text-gray-500">No devices found.</p>
          )}
        </CardContent>
      </Card>
    </div>
  );
}
