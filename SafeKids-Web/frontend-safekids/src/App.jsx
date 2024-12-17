import { BrowserRouter, Routes, Route } from "react-router-dom";
import Login from "./pages/Login";
import Dashboard from "./pages/Dashboard";
import DeviceDetails from "./pages/DeviceDetails";
import Layout from "./pages/Layout"; // Import Layout

function App() {
  return (
    <BrowserRouter>
      <Routes>
        {/* Route cho trang Login */}
        <Route path="/login" element={<Login />} />

        {/* Route Layout với Sidebar cố định */}
        <Route path="/" element={<Layout />}>
          <Route path="dashboard" element={<Dashboard />} />
          <Route path="device/:deviceId" element={<DeviceDetails />} />
          <Route path="settings" element={<div>Settings Page</div>} />
        </Route>

        {/* Route mặc định - Redirect đến login */}
        <Route path="*" element={<Login />} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
