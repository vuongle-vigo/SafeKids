import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom";
import Login from "./pages/Login";
import Devices from "./pages/Devices";
import Layout from "./pages/Layout"; // Import Layout
import DeviceManagement from "./pages/DeviceManagement";
import Admin from "./pages/Admin";
import UserDevices from "./pages/UserDevices";

function App() {
  return (
    <BrowserRouter>
      <Routes>
        {/* Route cho trang Login */}
        <Route path="/login" element={<Login />} />

        {/* Route Layout với Sidebar cố định */}
        <Route path="/" element={<Layout />}>
          <Route index element={<Navigate to="/devices" replace />} />
          <Route path="devices" element={<Devices />} />
          <Route path="device/:deviceId/manage" element={<DeviceManagement />} />
          <Route path="settings" element={<div>Settings Page</div>} />
          <Route path="/admin" element={<Admin />} />
          <Route path="/admin/user-devices/:userId" element={<UserDevices />} />
        </Route>

        {/* Route mặc định - Redirect đến login */}
        <Route path="*" element={<Login />} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
