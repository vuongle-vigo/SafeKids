import { Outlet, Link, useNavigate } from "react-router-dom";
import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Dialog, DialogContent, DialogHeader, DialogTitle, DialogFooter } from "@/components/ui/dialog";
import { Menu, X, Monitor, Settings2, LogOut } from "lucide-react";
import { cn } from "@/lib/utils";

export default function Layout() {
  const navigate = useNavigate();
  const [isSidebarOpen, setIsSidebarOpen] = useState(false);
  const [logoutDialogOpen, setLogoutDialogOpen] = useState(false);

  const handleLogout = () => {
    localStorage.removeItem("token");
    navigate("/login");
  };

  const toggleSidebar = () => {
    setIsSidebarOpen(!isSidebarOpen);
  };

  const closeSidebar = () => {
    setIsSidebarOpen(false);
  };

  const navItems = [
    { to: "/devices", label: "Thiết bị", icon: Monitor },
    { to: "/settings", label: "Cài đặt", icon: Settings2 },
  ];

  return (
    <div className="flex h-screen">
      {/* Sidebar */}
      <aside
        className={cn(
          "fixed top-0 left-0 h-full bg-background border-r shadow-sm transition-transform duration-300 z-30",
          isSidebarOpen ? "translate-x-0 w-64" : "-translate-x-full w-64",
          "flex flex-col justify-between p-4"
        )}
      >
        <div>
          <h2 className="text-xl font-bold text-center mb-8">SafeKids</h2>
          <nav className="space-y-2">
            {navItems.map((item) => (
              <Button
                key={item.to}
                variant="ghost"
                className="w-full justify-start text-sm"
                asChild
                onClick={closeSidebar}
              >
                <Link to={item.to} className="flex items-center">
                  <item.icon className="h-4 w-4 mr-2" />
                  {item.label}
                </Link>
              </Button>
            ))}
          </nav>
        </div>
        <Button
          variant="destructive"
          className="w-full"
          onClick={() => setLogoutDialogOpen(true)}
        >
          <LogOut className="h-4 w-4 mr-2" />
          Đăng xuất
        </Button>
      </aside>

      {/* Overlay */}
      {isSidebarOpen && (
        <div
          className="fixed inset-0 bg-black bg-opacity-50 z-20"
          onClick={closeSidebar}
        ></div>
      )}

      {/* Main Content */}
      <div className="flex-1 p-6">
        {/* Toggle Button */}
        <Button
          variant="outline"
          size="icon"
          className="fixed top-4 left-4 z-30"
          onClick={toggleSidebar}
        >
          {isSidebarOpen ? <X className="h-4 w-4" /> : <Menu className="h-4 w-4" />}
        </Button>
        <Outlet />
      </div>

      {/* Logout Confirmation Dialog */}
      <Dialog open={logoutDialogOpen} onOpenChange={setLogoutDialogOpen}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Xác nhận đăng xuất</DialogTitle>
          </DialogHeader>
          <div className="py-4">
            <p>Bạn có chắc chắn muốn đăng xuất khỏi SafeKids?</p>
          </div>
          <DialogFooter>
            <Button variant="outline" onClick={() => setLogoutDialogOpen(false)}>
              Hủy
            </Button>
            <Button variant="destructive" onClick={handleLogout}>
              Đăng xuất
            </Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>
    </div>
  );
}