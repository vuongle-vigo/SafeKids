import { Outlet, Link, useNavigate } from "react-router-dom";
import { FaDesktop, FaCog, FaBars } from "react-icons/fa"; // Import icons
import { useState } from "react";

export default function Layout() {
  const navigate = useNavigate();
  const [isSidebarOpen, setIsSidebarOpen] = useState(false);

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

  return (
    <div className="flex h-screen relative">
      {/* Sidebar */}
      <div
        className={`fixed top-0 left-0 h-full bg-gray-200 shadow-md p-4 flex flex-col justify-between transform ${
          isSidebarOpen ? "translate-x-0 w-60" : "-translate-x-full w-40"
        } transition-transform duration-300 z-30`}
      >
        <div>
          <h2 className="font-bold mb-6 text-center text-gray-800 text-lg">SafeKids</h2>
          <ul className="space-y-4">
            <li>
              <Link
                to="/devices"
                className="flex items-center text-sm px-3 py-2 rounded-md text-gray-700 hover:bg-gray-300 transition"
              >
                <FaDesktop className="mr-2" /> Devices
              </Link>
            </li>
            <li>
              <Link
                to="/settings"
                className="flex items-center text-sm px-3 py-2 rounded-md text-gray-700 hover:bg-gray-300 transition"
              >
                <FaCog className="mr-2" /> Settings
              </Link>
            </li>
          </ul>
        </div>

        {/* Logout */}
        <button
          onClick={handleLogout}
          className="w-full mt-4 px-4 py-2 bg-gray-400 text-white rounded-md hover:bg-gray-500 transition"
        >
          Logout
        </button>
      </div>

      {/* Overlay */}
      {isSidebarOpen && (
        <div
          className="fixed inset-0 bg-black bg-opacity-50 z-20"
          onClick={closeSidebar}
        ></div>
      )}

      {/* Sidebar Toggle Button */}
      <button
        onClick={toggleSidebar}
        className="absolute top-4 left-4 z-30 p-2 bg-blue-300 text-white rounded-md hover:bg-blue-400 transition"
      >
        <FaBars />
      </button>

      {/* Main Content */}
      <div className="flex-1 p-6">
        <Outlet /> {/* Nội dung của từng trang sẽ render ở đây */}
      </div>
    </div>
  );
}
