import { useState, useEffect } from "react";
import { useNavigate, useParams } from "react-router-dom";
import axios from "axios";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";

export default function UserDevices() {
  const [devices, setDevices] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");
  const navigate = useNavigate();
  const { userId } = useParams();

  useEffect(() => {
    const role = localStorage.getItem("role");
    if (role !== "admin") {
      navigate("/login");
      return;
    }

    const fetchDevices = async () => {
      try {
        const token = localStorage.getItem("token");
        console.log("Fetching devices for userId:", userId);
        const response = await axios.get(`/api/devices/${userId}/get-device`, {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        });
        console.log("Devices fetched:", response.data);
        setDevices(response.data.devices);
        setLoading(false);
      } catch (err) {
        setError(
          err.response?.data?.message || "Đã xảy ra lỗi khi lấy danh sách thiết bị"
        );
        setLoading(false);
        if (err.response?.status === 401) {
          localStorage.removeItem("token");
          localStorage.removeItem("role");
          navigate("/login");
        }
      }
    };

    fetchDevices();
  }, [navigate, userId]);

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen">
        <p className="text-lg text-gray-600">Đang tải...</p>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-100 p-6">
      <Card className="max-w-4xl mx-auto">
        <CardHeader>
          <CardTitle className="text-2xl font-bold text-gray-900">
            Danh sách thiết bị của người dùng
          </CardTitle>
        </CardHeader>
        <CardContent>
          {error && (
            <p className="text-sm text-red-500 bg-red-50 p-2 rounded-md mb-4">
              {error}
            </p>
          )}
          {devices.length === 0 ? (
            <p className="text-gray-500 text-center">Không có thiết bị nào.</p>
          ) : (
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>Tên thiết bị</TableHead>
                  <TableHead>Trạng thái</TableHead>
                  <TableHead>Ngày thêm</TableHead>
                  <TableHead>Hoạt động cuối</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {devices.map((device) => (
                  <TableRow key={device.device_id}>
                    <TableCell>{device.device_name}</TableCell>
                    <TableCell className={device.device_status === "online" ? "text-green-600" : "text-red-600"}>
                      {device.device_status.charAt(0).toUpperCase() + device.device_status.slice(1)}
                    </TableCell>
                    <TableCell>
                      {new Date(device.created_at).toLocaleString("vi-VN", {
                        dateStyle: "short",
                        timeStyle: "short",
                      })}
                    </TableCell>
                    <TableCell>
                      {device.last_activity
                        ? new Date(device.last_activity).toLocaleString("vi-VN", {
                            dateStyle: "short",
                            timeStyle: "short",
                          })
                        : "Chưa có"}
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          )}
          <Button
            className="mt-4 bg-blue-600 hover:bg-blue-700"
            onClick={() => navigate("/admin")}
          >
            Quay lại
          </Button>
        </CardContent>
      </Card>
    </div>
  );
}