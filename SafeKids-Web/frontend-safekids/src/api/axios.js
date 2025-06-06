import axios from "axios";
import { useNavigate } from "react-router-dom";

// Lấy URL gốc của API từ biến môi trường
const VITE_API_URL = import.meta.env.VITE_API_URL || "http://localhost:8889";

// Tạo một instance axios mới
const axiosInstance = axios.create({
  baseURL: VITE_API_URL,  // URL gốc của API
  timeout: 10000,  // Timeout nếu yêu cầu mất quá lâu
});

// Request Interceptor: Thêm token vào header
axiosInstance.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem("token");  // Lấy token từ localStorage
    if (token) {
      config.headers["Authorization"] = `Bearer ${token}`;  // Thêm token vào header của yêu cầu
    }
    return config;  // Trả về config đã được thêm token
  },
  (error) => {
    return Promise.reject(error);  // Nếu có lỗi trong quá trình cấu hình yêu cầu, trả về lỗi
  }
);

// Response Interceptor: Kiểm tra lỗi 401 và chuyển về trang login
axiosInstance.interceptors.response.use(
  (response) => {
    return response; // Nếu không có lỗi, trả về response như bình thường
  },
  (error) => {
    if (error.response && error.response.status === 401) {  // Nếu mã lỗi 401 (Unauthorized)
      localStorage.removeItem("token");  // Xoá token trong localStorage
      window.location.href = "/login";  // Điều hướng về trang login
    }
    return Promise.reject(error);  // Trả về lỗi nếu có
  }
);

export default axiosInstance;  // Export axiosInstance để sử dụng trong các file khác
