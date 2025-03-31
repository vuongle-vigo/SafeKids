import { BrowserRouter, Routes, Route } from "react-router-dom";
import Login from "./pages/Login";
import Devices from "./pages/Devices";
import Layout from "./pages/Layout"; // Import Layout
import DeviceManagement from "./pages/DeviceManagement";

function App() {
  return (
    <BrowserRouter>
      <Routes>
        {/* Route cho trang Login */}
        <Route path="/login" element={<Login />} />

        {/* Route Layout với Sidebar cố định */}
        <Route path="/" element={<Layout />}>
          <Route path="devices" element={<Devices />} />
          <Route path="device/:deviceId/manage" element={<DeviceManagement />} />
          <Route path="settings" element={<div>Settings Page</div>} />
        </Route>

        {/* Route mặc định - Redirect đến login */}
        <Route path="*" element={<Login />} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
