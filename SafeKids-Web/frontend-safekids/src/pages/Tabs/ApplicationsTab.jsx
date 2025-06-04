import { useEffect, useState } from "react";
import axios from "axios";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Button } from "@/components/ui/button";
import { Dialog, DialogContent, DialogHeader, DialogTitle, DialogFooter } from "@/components/ui/dialog";
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from "@/components/ui/table";
import { Tooltip, TooltipContent, TooltipProvider, TooltipTrigger } from "@/components/ui/tooltip";
import { Avatar, AvatarImage, AvatarFallback } from "@/components/ui/avatar";
import { Search } from "lucide-react";
import defaultIcon from "../../resource/default_icon.png";

export default function ApplicationsTab({ isActive, deviceId, onToggle, onUninstall }) {
  const [installedApps, setInstalledApps] = useState([]);
  const [searchTerm, setSearchTerm] = useState("");
  const [modalMessage, setModalMessage] = useState("");
  const [isModalVisible, setIsModalVisible] = useState(false);
  const [restrictedState, setRestrictedState] = useState({});
  const [blockedApps, setBlockedApps] = useState([]);
  const [uninstallApps, setUninstallApps] = useState([]);
  const [selectedApp, setSelectedApp] = useState(null);
  const [modalAction, setModalAction] = useState(null); // Track action (uninstall or unmark)

  useEffect(() => {
    if (!isActive || !deviceId) return;

    const fetchInstalledApps = async () => {
      try {
        const token = localStorage.getItem("token");
        const response = await axios.get(`/api/devices/${deviceId}/installed-apps`, {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        });
        setInstalledApps(response.data);
      } catch (error) {
        console.error("Error fetching installed apps:", error);
      }
    };

    fetchInstalledApps();
  }, [isActive, deviceId]);

  useEffect(() => {
    if (!isActive || !deviceId || installedApps.length === 0) return;

    const fetchConfig = async () => {
      try {
        const token = localStorage.getItem("token");
        const response = await axios.get(`/api/devices/${deviceId}/config`, {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        });
        const configApps = response.data[0]?.config_apps;
        if (configApps) {
          setBlockedApps(configApps.blocked || []);
          setUninstallApps(configApps.uninstall || []);

          // Initialize restrictedState based on blocked apps
          const initialRestrictedState = {};
          configApps.blocked.forEach((blockedApp) => {
            installedApps.forEach((app) => {
              if (blockedApp.app_id === app.install_location && blockedApp.app_name === app.app_name) {
                initialRestrictedState[app.app_name] = true;
              }
            });
          });
          setRestrictedState(initialRestrictedState);
        }
      } catch (error) {
        console.error("Error fetching config:", error);
      }
    };

    fetchConfig();
  }, [isActive, deviceId, installedApps]);

  const updateConfigApps = async (newBlockedApps, newUninstallApps) => {
    try {
      const token = localStorage.getItem("token");
      const updatedConfig = {
          blocked: newBlockedApps,
          uninstall: newUninstallApps,
      };
      await axios.put(`/api/devices/${deviceId}/update-app-config`, updatedConfig, {
        headers: {
          Authorization: `Bearer ${token}`,
        },
      });
      setBlockedApps(newBlockedApps);
      setUninstallApps(newUninstallApps);
    } catch (error) {
      console.error("Error updating config:", error);
    }
  };

  const handleToggleRestriction = async (app) => {
    const appName = app.app_name;
    const appId = app.install_location;
    let newBlockedApps;

    if (isBlocked(app)) {
      newBlockedApps = blockedApps.filter(
        (blockedApp) => !(blockedApp.app_id === appId && blockedApp.app_name === appName)
      );
    } else {
      newBlockedApps = [...blockedApps, { app_id: appId, app_name: appName }];
    }

    setRestrictedState((prevState) => ({
      ...prevState,
      [appName]: !prevState[appName],
    }));
    await updateConfigApps(newBlockedApps, uninstallApps);
    // onToggle(appName);
  };

  const handleUninstallClick = (app) => {
    if (app.quiet_uninstall_string === null) {
      setModalMessage(`Vui lòng gỡ cài đặt ${app.app_name} thủ công trên thiết bị.`);
      setIsModalVisible(true);
    } else {
      setModalMessage(`Bạn có chắc chắn muốn gỡ cài đặt ${app.app_name}? Ứng dụng sẽ được gỡ vào lần khởi động lại tiếp theo.`);
      setSelectedApp(app);
      setModalAction("uninstall");
      setIsModalVisible(true);
    }
  };

  const handleUnmarkUninstallClick = (app) => {
    setModalMessage(`Bạn có chắc chắn muốn bỏ đánh dấu gỡ cài đặt ${app.app_name}?`);
    setSelectedApp(app);
    setModalAction("unmark");
    setIsModalVisible(true);
  };

  const confirmAction = async () => {
    if (!selectedApp) return;

    if (modalAction === "uninstall") {
      const newUninstallApps = [
        ...uninstallApps,
        {
          app_name: selectedApp.app_name,
          quiet_uninstall_string: selectedApp.quiet_uninstall_string,
        },
      ];
      await updateConfigApps(blockedApps, newUninstallApps);
      // onUninstall(selectedApp.app_name);
      setModalMessage(`${selectedApp.app_name} đã được đánh dấu để gỡ cài đặt vào lần khởi động lại tiếp theo.`);
    } else if (modalAction === "unmark") {
      const newUninstallApps = uninstallApps.filter(
        (uninstallApp) => uninstallApp.app_name !== selectedApp.app_name
      );
      await updateConfigApps(blockedApps, newUninstallApps);
      setModalMessage(`${selectedApp.app_name} đã được bỏ đánh dấu gỡ cài đặt.`);
    }

    setSelectedApp(null);
    setModalAction(null);
  };

  const isBlocked = (app) => {
    return blockedApps.some(
      (blockedApp) => blockedApp.app_id === app.install_location && blockedApp.app_name === app.app_name
    );
  };

  const isMarkedForUninstall = (app) => {
    return uninstallApps.some((uninstallApp) => uninstallApp.app_name === app.app_name);
  };

  const filteredApps = installedApps.filter((app) =>
    app.app_name.toLowerCase().includes(searchTerm.toLowerCase())
  );

  return (
    <div className="space-y-6">
      {/* Search */}
      <Card>
        <CardContent className="pt-6">
          <div className="relative max-w-sm">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-muted-foreground" />
            <Input
              placeholder="Tìm kiếm ứng dụng..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="pl-10"
            />
          </div>
        </CardContent>
      </Card>

      {/* Applications Table */}
      <Card>
        <CardHeader>
          <CardTitle>Danh sách ứng dụng</CardTitle>
        </CardHeader>
        <CardContent>
          {filteredApps.length > 0 ? (
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead className="w-16">Icon</TableHead>
                  <TableHead>Tên ứng dụng</TableHead>
                  <TableHead>Phiên bản</TableHead>
                  <TableHead>Nhà phát hành</TableHead>
                  <TableHead className="text-right">Hành động</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {filteredApps.map((app) => (
                  <TableRow key={app.id}>
                    <TableCell>
                      <Avatar className="h-10 w-10">
                        <AvatarImage
                          src={app.app_icon ? `data:image/png;base64,${app.app_icon}` : defaultIcon}
                          alt={`${app.app_name} icon`}
                        />
                        <AvatarFallback>{app.app_name?.[0] || "N/A"}</AvatarFallback>
                      </Avatar>
                    </TableCell>
                    <TableCell>{app.app_name || "Không xác định"}</TableCell>
                    <TableCell>{app.version || "N/A"}</TableCell>
                    <TableCell>{app.publisher || "N/A"}</TableCell>
                    <TableCell className="text-right">
                      <div className="flex justify-end gap-2">
                        <TooltipProvider>
                          <Tooltip>
                            <TooltipTrigger asChild>
                              <Button
                                variant={isBlocked(app) ? "default" : "destructive"}
                                size="sm"
                                onClick={() => handleToggleRestriction(app)}
                              >
                                {isBlocked(app) ? "Bỏ hạn chế" : "Hạn chế"}
                              </Button>
                            </TooltipTrigger>
                            <TooltipContent>
                              {isBlocked(app) ? "Bỏ hạn chế ứng dụng này" : "Hạn chế ứng dụng này"}
                            </TooltipContent>
                          </Tooltip>
                        </TooltipProvider>
                        <TooltipProvider>
                          <Tooltip>
                            <TooltipTrigger asChild>
                              <Button
                                variant={isMarkedForUninstall(app) ? "outline" : "outline"}
                                size="sm"
                                onClick={() =>
                                  isMarkedForUninstall(app)
                                    ? handleUnmarkUninstallClick(app)
                                    : handleUninstallClick(app)
                                }
                              >
                                {isMarkedForUninstall(app) ? "Bỏ đánh dấu gỡ" : "Gỡ cài đặt"}
                              </Button>
                            </TooltipTrigger>
                            <TooltipContent>
                              {isMarkedForUninstall(app)
                                ? "Bỏ đánh dấu gỡ cài đặt ứng dụng"
                                : "Gỡ cài đặt ứng dụng"}
                            </TooltipContent>
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
              Không có thông tin ứng dụng nào được tìm thấy.
            </div>
          )}
        </CardContent>
      </Card>

      {/* Action Confirmation Dialog */}
      <Dialog open={isModalVisible} onOpenChange={setIsModalVisible}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Thông báo</DialogTitle>
          </DialogHeader>
          <div className="py-4">
            <p>{modalMessage}</p>
          </div>
          <DialogFooter>
            {selectedApp && modalMessage.includes("Bạn có chắc chắn") ? (
              <>
                <Button variant="outline" onClick={() => setIsModalVisible(false)}>
                  Hủy
                </Button>
                <Button onClick={confirmAction}>Xác nhận</Button>
              </>
            ) : (
              <Button onClick={() => setIsModalVisible(false)}>Đóng</Button>
            )}
          </DialogFooter>
        </DialogContent>
      </Dialog>
    </div>
  );
}