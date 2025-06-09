import { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
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
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
} from "@/components/ui/dialog";
import {
  Tabs,
  TabsContent,
  TabsList,
  TabsTrigger,
} from "@/components/ui/tabs";
import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "@/components/ui/form";
import { Input } from "@/components/ui/input";
import { Trash2, Edit, Smartphone } from "lucide-react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import * as z from "zod";

const formSchema = z.object({
  email: z.string().email("Email không hợp lệ"),
  password: z.string().min(6, "Mật khẩu phải có ít nhất 6 ký tự"),
});

const changePasswordSchema = z.object({
  newPassword: z.string().min(6, "Mật khẩu mới phải có ít nhất 6 ký tự"),
});

export default function Admin() {
  const [users, setUsers] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState("");
  const [dialog, setDialog] = useState({
    open: false,
    type: "", // "confirm", "success", "error", "add", "change-password"
    userId: null,
    title: "",
    message: "",
    role: "",
  });
  const navigate = useNavigate();

  const addUserForm = useForm({
    resolver: zodResolver(formSchema),
    defaultValues: {
      email: "",
      password: "",
    },
  });

  const changePasswordForm = useForm({
    resolver: zodResolver(changePasswordSchema),
    defaultValues: {
      newPassword: "",
    },
  });

  useEffect(() => {
    const role = localStorage.getItem("role");
    if (role !== "admin") {
      navigate("/login");
      return;
    }

    const fetchUsers = async () => {
      try {
        const token = localStorage.getItem("token");
        const response = await axios.get("/api/admin/get-all-users", {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        });
        setUsers(response.data.users);
        setLoading(false);
      } catch (err) {
        setError(
          err.response?.data?.message || "Đã xảy ra lỗi khi lấy danh sách người dùng"
        );
        setLoading(false);
        if (err.response?.status === 401) {
          localStorage.removeItem("token");
          localStorage.removeItem("role");
          navigate("/login");
        }
      }
    };

    fetchUsers();
  }, [navigate]);

  const handleDelete = async () => {
    try {
      const token = localStorage.getItem("token");
      console.log("Deleting user with ID:", dialog);
      await axios.delete(`/api/admin/delete-user/${dialog.userId}`, {
        headers: {
          Authorization: `Bearer ${token}`,
        },
      });
      setUsers(users.filter((user) => user.user_id !== dialog.userId));
      setDialog({
        open: true,
        type: "success",
        userId: null,
        title: "Thành công",
        message: "Xóa người dùng thành công",
      });
    } catch (err) {
      setDialog({
        open: true,
        type: "error",
        userId: null,
        title: "Lỗi",
        message: err.response?.data?.message || "Không thể xóa người dùng",
      });
    }
  };

  const openConfirmDialog = (userId) => {
    setDialog({
      open: true,
      type: "confirm",
      userId,
      title: "Xác nhận xóa",
      message: "Bạn có chắc muốn xóa người dùng này?",
      role: "",
    });
  };

  const openChangePasswordDialog = (userId) => {
    setDialog({
      open: true,
      type: "change-password",
      userId,
      title: "Đổi mật khẩu",
      message: "",
      role: "",
    });
    changePasswordForm.reset();
  };

  const handleChangePassword = async (data) => {
    try {
      const token = localStorage.getItem("token");
      await axios.put(
        `/api/admin/change-password/${dialog.userId}`,
        {
          newPassword: data.newPassword,
        },
        {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );
      setDialog({
        open: true,
        type: "success",
        userId: null,
        title: "Thành công",
        message: "Đổi mật khẩu thành công",
        role: "",
      });
      changePasswordForm.reset();
    } catch (err) {
      setDialog({
        open: true,
        type: "error",
        userId: null,
        title: "Lỗi",
        message: err.response?.data?.message || "Không thể đổi mật khẩu",
        role: "",
      });
    }
  };

  const handleViewDevices = (userId) => {
    navigate(`/admin/user-devices/${userId}`);
  };

  const openAddUserDialog = (role) => {
    setDialog({
      open: true,
      type: "add",
      userId: null,
      title: `Thêm tài khoản ${role === "parent" ? "phụ huynh" : "admin"}`,
      message: "",
      role,
    });
    addUserForm.reset();
  };

  const handleAddUser = async (data) => {
    try {
      const token = localStorage.getItem("token");
      const response = await axios.post(
        "/api/admin/create-user",
        {
          email: data.email,
          password: data.password,
          role: dialog.role,
        },
        {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );
      setUsers([...users, response.data.user]);
      setDialog({
        open: true,
        type: "success",
        userId: null,
        title: "Thành công",
        message: `Tạo tài khoản ${dialog.role === "parent" ? "phụ huynh" : "admin"} thành công`,
        role: "",
      });
      addUserForm.reset();
    } catch (err) {
      setDialog({
        open: true,
        type: "error",
        userId: null,
        title: "Lỗi",
        message: err.response?.data?.message || "Không thể tạo tài khoản",
        role: "",
      });
    }
  };

  const closeDialog = () => {
    setDialog({ open: false, type: "", userId: null, title: "", message: "", role: "" });
    addUserForm.reset();
    changePasswordForm.reset();
  };

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen">
        <p className="text-lg text-gray-600">Đang tải...</p>
      </div>
    );
  }

  // Lọc người dùng theo vai trò
  const parentUsers = users.filter((user) => user.role === "parent");
  const adminUsers = users.filter((user) => user.role === "admin");

  return (
    <div className="min-h-screen bg-gray-50 p-6">
      <Card className="max-w-5xl mx-auto shadow-lg">
        <CardHeader className="border-b">
          <CardTitle className="text-2xl font-semibold text-gray-800">
            Quản lý người dùng
          </CardTitle>
        </CardHeader>
        <CardContent className="p-6">
          {error && (
            <p className="text-sm text-red-500 bg-red-50 p-3 rounded-md mb-4">
              {error}
            </p>
          )}
          <Tabs defaultValue="parent" className="w-full">
            <TabsList className="grid w-full grid-cols-2 mb-6">
              <TabsTrigger value="parent">Quản lý phụ huynh</TabsTrigger>
              <TabsTrigger value="admin">Quản lý admin</TabsTrigger>
            </TabsList>
            <TabsContent value="parent">
              {parentUsers.length === 0 ? (
                <p className="text-gray-500 text-center py-4">
                  Không có phụ huynh nào.
                </p>
              ) : (
                <Table>
                  <TableHeader>
                    <TableRow>
                      <TableHead className="font-medium">Email</TableHead>
                      <TableHead className="font-medium">Tên người dùng</TableHead>
                      <TableHead className="font-medium">Vai trò</TableHead>
                      <TableHead className="text-right font-medium">Hành động</TableHead>
                    </TableRow>
                  </TableHeader>
                  <TableBody>
                    {parentUsers.map((user) => (
                      <TableRow key={user.user_id} className="hover:bg-gray-50">
                        <TableCell>{user.email}</TableCell>
                        <TableCell>{user.username}</TableCell>
                        <TableCell>{user.role}</TableCell>
                        <TableCell className="text-right space-x-2">
                          <Button
                            variant="outline"
                            size="sm"
                            onClick={() => openChangePasswordDialog(user.user_id)}
                            className="text-blue-600 hover:text-blue-800 border-blue-200"
                          >
                            <Edit className="h-4 w-4 mr-1" />
                            Đổi mật khẩu
                          </Button>
                          <Button
                            variant="outline"
                            size="sm"
                            onClick={() => handleViewDevices(user.user_id)}
                            className="text-green-600 hover:text-green-800 border-green-200"
                          >
                            <Smartphone className="h-4 w-4 mr-1" />
                            Quản lý thiết bị
                          </Button>
                          <Button
                            variant="outline"
                            size="sm"
                            onClick={() => openConfirmDialog(user.user_id)}
                            className="text-red-500 hover:text-red-700 border-red-200"
                          >
                            <Trash2 className="h-4 w-4 mr-1" />
                            Xóa
                          </Button>
                        </TableCell>
                      </TableRow>
                    ))}
                  </TableBody>
                </Table>
              )}
              <Button
                className="mt-6 bg-blue-600 hover:bg-blue-700 text-white"
                onClick={() => openAddUserDialog("parent")}
              >
                Thêm phụ huynh
              </Button>
            </TabsContent>
            <TabsContent value="admin">
              {adminUsers.length === 0 ? (
                <p className="text-gray-500 text-center py-4">
                  Không có admin nào.
                </p>
              ) : (
                <Table>
                  <TableHeader>
                    <TableRow>
                      <TableHead className="font-medium">Email</TableHead>
                      <TableHead className="font-medium">Tên người dùng</TableHead>
                      <TableHead className="font-medium">Vai trò</TableHead>
                      <TableHead className="text-right font-medium">Hành động</TableHead>
                    </TableRow>
                  </TableHeader>
                  <TableBody>
                    {adminUsers.map((user) => (
                      <TableRow key={user.user_id} className="hover:bg-gray-50">
                        <TableCell>{user.email}</TableCell>
                        <TableCell>{user.username}</TableCell>
                        <TableCell>{user.role}</TableCell>
                        <TableCell className="text-right space-x-2">
                          <Button
                            variant="outline"
                            size="sm"
                            onClick={() => openChangePasswordDialog(user.user_id)}
                            className="text-blue-600 hover:text-blue-800 border-blue-200"
                          >
                            <Edit className="h-4 w-4 mr-1" />
                            Đổi mật khẩu
                          </Button>
                          <Button
                            variant="outline"
                            size="sm"
                            onClick={() => openConfirmDialog(user.user_id)}
                            className="text-red-500 hover:text-red-700 border-red-200"
                          >
                            <Trash2 className="h-4 w-4 mr-1" />
                            Xóa
                          </Button>
                        </TableCell>
                      </TableRow>
                    ))}
                  </TableBody>
                </Table>
              )}
              <Button
                className="mt-6 bg-blue-600 hover:bg-blue-700 text-white"
                onClick={() => openAddUserDialog("admin")}
              >
                Thêm admin
              </Button>
            </TabsContent>
          </Tabs>
        </CardContent>
      </Card>

      <Dialog open={dialog.open} onOpenChange={closeDialog}>
        <DialogContent className="sm:max-w-md">
          <DialogHeader>
            <DialogTitle className="text-lg font-semibold">{dialog.title}</DialogTitle>
            {dialog.type !== "add" && dialog.type !== "change-password" && (
              <DialogDescription className="text-gray-600">{dialog.message}</DialogDescription>
            )}
          </DialogHeader>
          {dialog.type === "add" && (
            <Form {...addUserForm}>
              <form onSubmit={addUserForm.handleSubmit(handleAddUser)} className="space-y-4">
                <FormField
                  control={addUserForm.control}
                  name="email"
                  render={({ field }) => (
                    <FormItem>
                      <FormLabel>Email</FormLabel>
                      <FormControl>
                        <Input placeholder="nhap@email.com" {...field} />
                      </FormControl>
                      <FormMessage />
                    </FormItem>
                  )}
                />
                <FormField
                  control={addUserForm.control}
                  name="password"
                  render={({ field }) => (
                    <FormItem>
                      <FormLabel>Mật khẩu</FormLabel>
                      <FormControl>
                        <Input type="password" placeholder="******" {...field} />
                      </FormControl>
                      <FormMessage />
                    </FormItem>
                  )}
                />
                <DialogFooter className="sm:justify-end gap-2">
                  <Button variant="outline" type="button" onClick={closeDialog}>
                    Hủy
                  </Button>
                  <Button type="submit">Thêm</Button>
                </DialogFooter>
              </form>
            </Form>
          )}
          {dialog.type === "change-password" && (
            <Form {...changePasswordForm}>
              <form onSubmit={changePasswordForm.handleSubmit(handleChangePassword)} className="space-y-4">
                <FormField
                  control={changePasswordForm.control}
                  name="newPassword"
                  render={({ field }) => (
                    <FormItem>
                      <FormLabel>Mật khẩu mới</FormLabel>
                      <FormControl>
                        <Input type="password" placeholder="******" {...field} />
                      </FormControl>
                      <FormMessage />
                    </FormItem>
                  )}
                />
                <DialogFooter className="sm:justify-end gap-2">
                  <Button variant="outline" type="button" onClick={closeDialog}>
                    Hủy
                  </Button>
                  <Button type="submit">Lưu</Button>
                </DialogFooter>
              </form>
            </Form>
          )}
          {dialog.type === "confirm" && (
            <DialogFooter className="sm:justify-end gap-2">
              <Button variant="outline" onClick={closeDialog}>
                Hủy
              </Button>
              <Button variant="destructive" onClick={handleDelete}>
                Xóa
              </Button>
            </DialogFooter>
          )}
          {(dialog.type === "success" || dialog.type === "error") && (
            <DialogFooter>
              <Button onClick={closeDialog} className="w-full sm:w-auto">
                Đóng
              </Button>
            </DialogFooter>
          )}
        </DialogContent>
      </Dialog>
    </div>
  );
}