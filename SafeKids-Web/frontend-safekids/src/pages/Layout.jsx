import { Outlet, Link, useNavigate } from "react-router-dom";
import { FaDesktop, FaCog } from "react-icons/fa"; // Import icons

export default function Layout() {
  const navigate = useNavigate();

  const handleLogout = () => {
    localStorage.removeItem("token");
    navigate("/login");
  };

  return (
    <div className="flex h-screen">
      {/* Sidebar */}
      <div className="w-40 bg-gray-200 shadow-md p-4 flex flex-col justify-between">
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

      {/* Main Content */}
      <div className="flex-1 p-6">
        <Outlet /> {/* Nội dung của từng trang sẽ render ở đây */}
      </div>
    </div>
  );
}
