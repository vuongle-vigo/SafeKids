import { Outlet, Link, useNavigate } from "react-router-dom";

export default function Layout() {
  const navigate = useNavigate();

  const handleLogout = () => {
    localStorage.removeItem("token");
    navigate("/login");
  };

  return (
    <div className="flex h-screen">
      {/* Sidebar */}
      <div className="w-30 bg-gray-100 shadow-md p-4 flex flex-col justify-between">
        <div>
          <h2 className="font-bold mb-6 text-center">SafeKids</h2>
          <ul className="space-y-4">
            <li>
              <Link
                to="/dashboard"
                className="block text-sm px-3 py-2 rounded-md hover:bg-gray-200"
              >
                Devices
              </Link>
            </li>
            <li>
              <Link
                to="/settings"
                className="block text-sm px-3 py-2 rounded-md hover:bg-gray-200"
              >
                Settings
              </Link>
            </li>
          </ul>
        </div>

        {/* Logout */}
        <button
          onClick={handleLogout}
          className="w-full mt-4 px-4 py-2 bg-blue-500 text-white rounded-md hover:bg-red-600 transition"
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
