import { useState, useEffect } from "react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Label } from "@/components/ui/label";
import { Eye, EyeOff } from "lucide-react";
import { useNavigate } from "react-router-dom";
import axios from "axios";

export default function Login() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const navigate = useNavigate();

  useEffect(() => {
    const token = localStorage.getItem("token");
    if (token) {
      axios
        .get("/api/devices/", {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        })
        .then(() => {
          navigate("/devices");
        })
        .catch(() => {
          localStorage.removeItem("token");
          navigate("/login");
        });
    }
  }, [navigate]);

  const handleSubmit = async (e) => {
    e.preventDefault();
    setError("");

    const credentials = btoa(`${username}:${password}`);

    try {
      const response = await axios.post("/api/auth/login", {}, {
        headers: {
          Authorization: `Basic ${credentials}`,
        },
      });

      localStorage.setItem("token", response.data.token);
      navigate("/devices");
    } catch (err) {
      setError(err.response?.data?.message || "Đã xảy ra lỗi trong quá trình đăng nhập");
    }
  };

  const togglePasswordVisibility = () => {
    setShowPassword(!showPassword);
  };

  return (
    <div className="relative flex items-center justify-center min-h-screen bg-gradient-to-r from-blue-600 via-indigo-600 to-purple-600 overflow-hidden">
      {/* Background Decorative Elements */}
      <div className="absolute inset-0 z-0">
        <div className="absolute top-[-100px] left-[-100px] w-[300px] h-[300px] bg-blue-400/20 rounded-full blur-3xl animate-pulse" />
        <div className="absolute bottom-[-150px] right-[-150px] w-[400px] h-[400px] bg-purple-400/20 rounded-full blur-3xl animate-pulse" />
      </div>

      {/* Login Card */}
      <Card className="relative z-10 w-full max-w-md p-6 bg-white/95 backdrop-blur-sm shadow-2xl rounded-xl border border-gray-100 animate-in fade-in-50">
        <CardHeader className="text-center space-y-2">
          <div className="flex justify-center mb-4">
            <div className="w-16 h-16 bg-blue-600 rounded-full flex items-center justify-center text-white text-2xl font-bold">
              SK
            </div>
          </div>
          <CardTitle className="text-3xl font-bold text-gray-900">
            SafeKids
          </CardTitle>
          <CardDescription className="text-gray-500 text-base">
            Đăng nhập để quản lý an toàn cho trẻ em
          </CardDescription>
        </CardHeader>
        <CardContent>
          <form onSubmit={handleSubmit} className="space-y-6">
            <div className="space-y-2">
              <Label htmlFor="username" className="text-sm font-medium text-gray-700">
                Email
              </Label>
              <Input
                id="username"
                placeholder="Nhập email"
                value={username}
                onChange={(e) => setUsername(e.target.value)}
                className="h-11 text-base transition-all duration-200 focus:ring-2 focus:ring-blue-500 focus:border-blue-500"
                autoComplete="username"
              />
            </div>
            <div className="space-y-2">
              <Label htmlFor="password" className="text-sm font-medium text-gray-700">
                Mật khẩu
              </Label>
              <div className="relative">
                <Input
                  id="password"
                  type={showPassword ? "text" : "password"}
                  placeholder="Nhập mật khẩu"
                  value={password}
                  onChange={(e) => setPassword(e.target.value)}
                  className="h-11 text-base pr-12 transition-all duration-200 focus:ring-2 focus:ring-blue-500 focus:border-blue-500"
                  autoComplete="current-password"
                />
                <Button
                  type="button"
                  variant="ghost"
                  size="icon"
                  className="absolute right-2 top-1/2 -translate-y-1/2 h-8 w-8 text-gray-500 hover:text-gray-700"
                  onClick={togglePasswordVisibility}
                  aria-label={showPassword ? "Ẩn mật khẩu" : "Hiện mật khẩu"}
                >
                  {showPassword ? <EyeOff className="h-5 w-5" /> : <Eye className="h-5 w-5" />}
                </Button>
              </div>
            </div>
            {error && (
              <p className="text-sm text-red-500 bg-red-50 p-2 rounded-md text-center">
                {error}
              </p>
            )}
            <Button
              type="submit"
              className="w-full h-11 bg-blue-600 text-white text-base font-semibold rounded-lg hover:bg-blue-700 transition-all duration-200"
            >
              Đăng nhập
            </Button>
          </form>
          <div className="mt-4 text-center">
            <a
              href="/forgot-password"
              className="text-sm text-blue-600 hover:underline"
            >
              Quên mật khẩu?
            </a>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}