import { useState, useEffect } from "react";
import axiosInstance from "../api/axios";
import { useNavigate, Link } from "react-router-dom";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Button } from "@/components/ui/button";
import { Dialog, DialogContent, DialogHeader, DialogTitle, DialogFooter } from "@/components/ui/dialog";
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from "@/components/ui/table";
import { Tooltip, TooltipContent, TooltipProvider, TooltipTrigger } from "@/components/ui/tooltip";
import { Badge } from "@/components/ui/badge";
import { Skeleton } from "@/components/ui/skeleton";
import { Label } from "@/components/ui/label";
import { Search, Edit2, Trash2 } from "lucide-react";

export default function Dashboard() {
  const [devices, setDevices] = useState([]);
  const [searchTerm, setSearchTerm] = useState("");
  const [isLoading, setIsLoading] = useState(true);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [dialogContent, setDialogContent] = useState({ title: "", message: "", action: null });
  const [editDialogOpen, setEditDialogOpen] = useState(false);
  const [editingDevice, setEditingDevice] = useState(null);
  const [newDeviceName, setNewDeviceName] = useState("");
  const navigate = useNavigate();

  useEffect(() => {
    const fetchDevices = async () => {
      setIsLoading(true);
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
      } finally {
        setIsLoading(false);
      }
    };

    fetchDevices();
  }, [navigate]);

  const isDeviceOnline = (device) => device.device_status === "online";

  const handleEditClick = (device) => {
    setEditingDevice(device);
    setNewDeviceName(device.device_name);
    setEditDialogOpen(true);
  };

  const handleSaveClick = async () => {
    if (!newDeviceName.trim()) {
      setDialogContent({
        title: "Lỗi",
        message: "Tên thiết bị không được để trống.",
        action: null,
      });
      setDialogOpen(true);
      return;
    }
    try {
      await axiosInstance.put(`/api/devices/${editingDevice.device_id}/update-name`, { device_name: newDeviceName });
      setDevices((prevDevices) =>
        prevDevices.map((device) =>
          device.device_id === editingDevice.device_id ? { ...device, device_name: newDeviceName } : device
        )
      );
      setEditDialogOpen(false);
      setEditingDevice(null);
      setNewDeviceName("");
    } catch (error) {
      console.error("Failed to update device name:", error);
      setDialogContent({
        title: "Lỗi",
        message: "Không thể cập nhật tên thiết bị. Vui lòng thử lại.",
        action: null,
      });
      setDialogOpen(true);
    }
  };

  const handleCancelEdit = () => {
    setEditDialogOpen(false);
    setEditingDevice(null);
    setNewDeviceName("");
  };

  const handleDeleteClick = (deviceId) => {
    setDialogContent({
      title: "Xác nhận xóa",
      message: "Bạn có chắc chắn muốn xóa thiết bị này?",
      action: () => confirmDelete(deviceId),
    });
    setDialogOpen(true);
  };

  const confirmDelete = async (deviceId) => {
    try {
      await axiosInstance.delete(`/api/devices/${deviceId}`);
      setDevices((prevDevices) =>
        prevDevices.filter((device) => device.device_id !== deviceId)
      );
      setDialogOpen(false);
    } catch (error) {
      console.error("Failed to delete device:", error);
      setDialogContent({
        title: "Lỗi",
        message: "Không thể xóa thiết bị. Vui lòng thử lại.",
        action: null,
      });
      setDialogOpen(true);
    }
  };

  const filteredDevices = devices.filter((device) =>
    device.device_name.toLowerCase().includes(searchTerm.toLowerCase())
  );

  return (
    <div className="flex-1 p-6 space-y-6">
      <Card>
        <CardHeader>
          <CardTitle>Quản lý thiết bị</CardTitle>
        </CardHeader>
        <CardContent>
          {/* Search */}
          <div className="relative max-w-sm mb-6">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-muted-foreground" />
            <Input
              placeholder="Tìm kiếm thiết bị..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="pl-10"
            />
          </div>

          {/* Devices Table */}
          {isLoading ? (
            <div className="space-y-2">
              <Skeleton className="h-8 w-full" />
              <Skeleton className="h-8 w-full" />
              <Skeleton className="h-8 w-full" />
            </div>
          ) : filteredDevices.length > 0 ? (
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>Tên thiết bị</TableHead>
                  <TableHead>Ngày tạo</TableHead>
                  <TableHead>Hoạt động cuối</TableHead>
                  <TableHead>Trạng thái</TableHead>
                  <TableHead className="text-right">Hành động</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {filteredDevices.map((device) => (
                  <TableRow key={device.device_id}>
                    <TableCell>
                      <span className="truncate max-w-xs" title={device.device_name}>
                        {device.device_name}
                      </span>
                    </TableCell>
                    <TableCell>
                      {new Date(device.created_at).toLocaleString("vi-VN", {
                        day: "2-digit",
                        month: "2-digit",
                        year: "numeric",
                        hour: "2-digit",
                        minute: "2-digit",
                      })}
                    </TableCell>
                    <TableCell>
                      {device.last_activity
                        ? new Date(device.last_activity).toLocaleString("vi-VN", {
                            day: "2-digit",
                            month: "2-digit",
                            year: "numeric",
                            hour: "2-digit",
                            minute: "2-digit",
                          })
                        : "N/A"}
                    </TableCell>
                    <TableCell>
                      <Badge variant={isDeviceOnline(device) ? "default" : "secondary"}
                      className={isDeviceOnline(device) ? "bg-green-500" : "bg-gray-500"}>
                        {/* <Badge
                variant={device.device_status ? "default" : "secondary"}
                className={device.device_status ? "bg-green-500" : "bg-gray-500"}
              ></Badge> */}
                        {isDeviceOnline(device) ? "Online" : "Offline"}
                      </Badge>
                    </TableCell>
                    <TableCell className="text-right">
                      <div className="flex justify-end gap-2">
                        <TooltipProvider>
                          <Tooltip>
                            <TooltipTrigger asChild>
                              <Button
                                variant="outline"
                                size="icon"
                                onClick={() => handleEditClick(device)}
                              >
                                <Edit2 className="h-4 w-4" />
                              </Button>
                            </TooltipTrigger>
                            <TooltipContent>Chỉnh sửa tên thiết bị</TooltipContent>
                          </Tooltip>
                        </TooltipProvider>
                        <TooltipProvider>
                          <Tooltip>
                            <TooltipTrigger asChild>
                              <Button
                                variant="outline"
                                size="icon"
                                onClick={() => handleDeleteClick(device.device_id)}
                              >
                                <Trash2 className="h-4 w-4" />
                              </Button>
                            </TooltipTrigger>
                            <TooltipContent>Xóa thiết bị</TooltipContent>
                          </Tooltip>
                        </TooltipProvider>
                        <TooltipProvider>
                          <Tooltip>
                            <TooltipTrigger asChild>
                              <Button
                                variant="outline"
                                size="sm"
                                asChild
                              >
                                <Link to={`/device/${device.device_id}/manage`}>
                                  Xem chi tiết
                                </Link>
                              </Button>
                            </TooltipTrigger>
                            <TooltipContent>Xem chi tiết thiết bị</TooltipContent>
                          </Tooltip>
                        </TooltipProvider>
                      </div>
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          ) : (
            <div className="text-center text-muted-foreground py-4">
              Không tìm thấy thiết bị nào.
            </div>
          )}
        </CardContent>
      </Card>

      {/* Edit Device Dialog */}
      <Dialog open={editDialogOpen} onOpenChange={setEditDialogOpen}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Chỉnh sửa tên thiết bị</DialogTitle>
          </DialogHeader>
          <div className="grid gap-4 py-4">
            <div className="grid grid-cols-4 items-center gap-4">
              <Label htmlFor="device-name" className="text-right">
                Tên thiết bị
              </Label>
              <Input
                id="device-name"
                value={newDeviceName}
                onChange={(e) => setNewDeviceName(e.target.value)}
                className="col-span-3"
                placeholder="Nhập tên thiết bị"
              />
            </div>
          </div>
          <DialogFooter>
            <Button variant="outline" onClick={handleCancelEdit}>
              Hủy
            </Button>
            <Button onClick={handleSaveClick}>Xác nhận</Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>

      {/* Notification Dialog */}
      <Dialog open={dialogOpen} onOpenChange={setDialogOpen}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>{dialogContent.title}</DialogTitle>
          </DialogHeader>
          <div className="py-4">
            <p>{dialogContent.message}</p>
          </div>
          <DialogFooter>
            {dialogContent.action ? (
              <>
                <Button variant="outline" onClick={() => setDialogOpen(false)}>
                  Hủy
                </Button>
                <Button
                  onClick={() => {
                    dialogContent.action();
                    setDialogOpen(false);
                  }}
                >
                  Xác nhận
                </Button>
              </>
            ) : (
              <Button onClick={() => setDialogOpen(false)}>Đóng</Button>
            )}
          </DialogFooter>
        </DialogContent>
      </Dialog>
    </div>
  );
}