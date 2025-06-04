import { useState, useEffect } from "react";
import { Bar } from "react-chartjs-2";
import axios from "axios";
import "chart.js/auto";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Tabs, TabsList, TabsTrigger } from "@/components/ui/tabs";
import {
  Dialog,
  DialogContent,
  DialogHeader,
  DialogTitle,
  DialogFooter,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Trash2 } from "lucide-react";

export default function PowerUsageTab({ screenTimeLimit, usageData, onUpdate, deviceId }) {
  const [selectedRange, setSelectedRange] = useState("today");
  const [showWeeklySettings, setShowWeeklySettings] = useState(false);
  const [weeklyLimits, setWeeklyLimits] = useState({
    Monday: 4,
    Tuesday: 4,
    Wednesday: 4,
    Thursday: 4,
    Friday: 4,
    Saturday: 6,
    Sunday: 6,
  });
  const [allowedTimes, setAllowedTimes] = useState({
    Monday: ["08:00-12:00", "14:00-22:00"],
    Tuesday: ["08:00-12:00", "14:00-22:00"],
    Wednesday: ["08:00-12:00", "14:00-22:00"],
    Thursday: ["08:00-12:00", "14:00-22:00"],
    Friday: ["08:00-12:00", "14:00-22:00"],
    Saturday: ["08:00-12:00", "14:00-22:00"],
    Sunday: ["08:00-12:00", "14:00-22:00"],
  });
  const [fetchedUsageData, setFetchedUsageData] = useState([]);
  const [selectedDay, setSelectedDay] = useState("Monday");
  const [dailyLimit, setDailyLimit] = useState({ hours: 4, minutes: 0 });
  const [tempAllowedTimes, setTempAllowedTimes] = useState(allowedTimes.Monday);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [dialogContent, setDialogContent] = useState({ title: "", message: "" });

  useEffect(() => {
    const fetchConfig = async () => {
      try {
        const token = localStorage.getItem("token");
        const response = await axios.get(`/api/devices/${deviceId}/config`, {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        });
        const config = response.data[0]?.time_limit_daily;
        if (config) {
          const dayOrder = [
            "sunday",
            "monday",
            "tuesday",
            "wednesday",
            "thursday",
            "friday",
            "saturday",
          ];

          const updatedLimits = {};
          const updatedAllowedTimes = {};

          dayOrder.forEach((day) => {
            if (config[day]) {
              const dayKey = day.charAt(0).toUpperCase() + day.slice(1);
              updatedLimits[dayKey] = config[day].max_hours;
              updatedAllowedTimes[dayKey] = config[day].allowed_time;
            }
          });

          setWeeklyLimits(updatedLimits);
          setAllowedTimes(updatedAllowedTimes);
        }
      } catch (error) {
        console.error("Error fetching config:", error);
      }
    };

    fetchConfig();
  }, [deviceId]);

  useEffect(() => {
    const fetchUsageData = async () => {
      const days = selectedRange === "today" ? 1 : selectedRange === "7days" ? 7 : selectedRange === "15days" ? 15 : 30;
      const end = new Date();
      const start = new Date();
      start.setDate(end.getDate() - (days - 1));
      const startDate = start.toISOString().split("T")[0];
      const endDate = end.toISOString().split("T")[0];

      try {
        const token = localStorage.getItem("token");
        const response = await axios.post(
          `/api/devices/${deviceId}/power-usage/time`,
          {
            startTime: startDate,
            endTime: endDate,
          },
          {
            headers: {
              Authorization: `Bearer ${token}`,
            },
          }
        );
        setFetchedUsageData(response.data);
      } catch (error) {
        console.error("Error fetching usage data:", error);
      }
    };

    fetchUsageData();
  }, [selectedRange, deviceId]);

  useEffect(() => {
    setSelectedRange("today");
  }, []);

  useEffect(() => {
    setDailyLimit({
      hours: Math.floor(weeklyLimits[selectedDay]),
      minutes: (weeklyLimits[selectedDay] % 1) * 60,
    });
    setTempAllowedTimes([...(allowedTimes[selectedDay] || [])]);
  }, [selectedDay, weeklyLimits, allowedTimes]);

  const handleRangeChange = (range) => {
    setSelectedRange(range);
  };

  const handleWeeklyLimitChange = (day, value) => {
    setWeeklyLimits((prev) => ({ ...prev, [day]: value }));
  };

  const handleDayClick = (day) => {
    setSelectedDay(day);
  };

  const handleDailyLimitChange = (field, value) => {
    const updatedValue = Math.max(0, Math.min(field === "hours" ? 24 : 59, Math.floor(value)));
    setDailyLimit((prev) => ({ ...prev, [field]: updatedValue }));
  };

  const handleAllowedTimeChange = (index, field, value) => {
    const updatedTimes = [...tempAllowedTimes];
    const [start, end] = updatedTimes[index].split("-");
    const newTime = field === "start" ? `${value}-${end}` : `${start}-${value}`;
    if (/^\d{2}:\d{2}-\d{2}:\d{2}$/.test(newTime)) {
      updatedTimes[index] = newTime;
      setTempAllowedTimes(updatedTimes);
    }
  };

  const addAllowedTime = () => {
    setTempAllowedTimes([...tempAllowedTimes, "00:00-00:00"]);
  };

  const removeAllowedTime = (index) => {
    setTempAllowedTimes(tempAllowedTimes.filter((_, i) => i !== index));
  };

  const saveConfigToBackend = async (updatedLimits, updatedAllowedTimes, setDialogOpen, setDialogContent) => {
    try {
      const token = localStorage.getItem("token");
      const configData = {
        time_limit_daily: Object.keys(updatedLimits).reduce((acc, day) => {
          acc[day.toLowerCase()] = {
            max_hours: updatedLimits[day],
            allowed_time: updatedAllowedTimes[day] || [],
          };
          return acc;
        }, {}),
      };
      console.log("Dữ liệu gửi đi:", configData);
      await axios.put(`/api/devices/${deviceId}/update-time-limit-config`, configData.time_limit_daily, {
        headers: {
          Authorization: `Bearer ${token}`,
        },
      });
      console.log("Cấu hình đã được lưu lên backend thành công:", configData);
      setDialogContent({
        title: "Thành công",
        message: "Cấu hình đã được lưu thành công!",
      });
      setDialogOpen(true);
    } catch (error) {
      console.error("Lỗi khi lưu cấu hình:", error.response?.data || error.message);
      setDialogContent({
        title: "Lỗi",
        message: "Lỗi khi lưu cấu hình: " + (error.response?.data?.message || error.message),
      });
      setDialogOpen(true);
    }
  };

  const saveDailyLimit = () => {
    const totalHours = dailyLimit.hours + dailyLimit.minutes / 60;
    const updatedLimits = { ...weeklyLimits, [selectedDay]: totalHours };
    const updatedAllowedTimes = { ...allowedTimes, [selectedDay]: tempAllowedTimes };
    console.log("Cập nhật:", { selectedDay, totalHours, tempAllowedTimes });
    setWeeklyLimits(updatedLimits);
    setAllowedTimes(updatedAllowedTimes);
    saveConfigToBackend(updatedLimits, updatedAllowedTimes, setDialogOpen, setDialogContent);
    setSelectedDay(null);
    setShowWeeklySettings(false);
  };

  const generateChartData = () => {
    const getGradient = (ctx, chartArea) => {
      const gradient = ctx.createLinearGradient(0, chartArea.bottom, 0, chartArea.top);
      gradient.addColorStop(0, "rgba(59, 130, 246, 0.4)");
      gradient.addColorStop(1, "rgba(59, 130, 246, 0.8)");
      return gradient;
    };

    if (selectedRange === "today") {
      return {
        labels: Array.from({ length: 24 }, (_, i) => `${i}:00`),
        datasets: [
          {
            label: "Phút sử dụng",
            data: Array.from({ length: 24 }, (_, i) =>
              fetchedUsageData
                .filter((data) => data.hour === i)
                .reduce((sum, data) => sum + data.usage_minutes, 0)
            ),
            backgroundColor: (context) => {
              const chart = context.chart;
              const { ctx, chartArea } = chart;
              if (!chartArea) return "rgba(59, 130, 246, 0.6)";
              return getGradient(ctx, chartArea);
            },
            borderColor: "rgba(59, 130, 246, 1)",
            borderWidth: 1,
            hoverBackgroundColor: "rgba(59, 130, 246, 1)",
            hoverBorderColor: "rgba(59, 130, 246, 1)",
          },
        ],
      };
    } else {
      const days = selectedRange === "7days" ? 7 : selectedRange === "15days" ? 15 : 30;
      const dateLabels = Array.from({ length: days }, (_, i) => {
        const date = new Date();
        date.setDate(date.getDate() - i);
        return date.toLocaleDateString("vi-VN", { day: "2-digit", month: "2-digit" });
      }).reverse();

      return {
        labels: dateLabels,
        datasets: [
          {
            label: "Giờ sử dụng",
            data: dateLabels.map((date) =>
              fetchedUsageData
                .filter((data) => {
                  const dataDate = new Date(data.date).toLocaleDateString("vi-VN", {
                    day: "2-digit",
                    month: "2-digit",
                  });
                  return dataDate === date;
                })
                .reduce((sum, data) => sum + data.usage_minutes / 60, 0)
            ),
            backgroundColor: (context) => {
              const chart = context.chart;
              const { ctx, chartArea } = chart;
              if (!chartArea) return "rgba(75, 192, 192, 0.6)";
              return getGradient(ctx, chartArea);
            },
            borderColor: "rgba(75, 192, 192, 1)",
            borderWidth: 1,
            hoverBackgroundColor: "rgba(75, 192, 192, 1)",
            hoverBorderColor: "rgba(75, 192, 192, 1)",
          },
        ],
      };
    }
  };

  const calculateSummary = () => {
    const filteredData = selectedRange === "today"
      ? fetchedUsageData
      : fetchedUsageData.filter((data) => {
          const dataDate = new Date(data.date);
          const rangeStart = new Date(new Date().setDate(new Date().getDate() - (selectedRange === "7days" ? 6 : selectedRange === "15days" ? 14 : 29)));
          const rangeEnd = new Date();
          return dataDate >= rangeStart && dataDate <= rangeEnd;
        });

    const totalMinutes = filteredData.reduce((sum, data) => sum + data.usage_minutes, 0);
    const totalHours = (totalMinutes / 60).toFixed(2);
    const daysInRange = selectedRange === "7days" ? 7 : selectedRange === "15days" ? 15 : selectedRange === "30days" ? 30 : 1;
    const averageHours = (totalMinutes / 60 / daysInRange).toFixed(2);
    const maxMinutes = Math.max(...filteredData.map((data) => data.usage_minutes), 0);
    const maxHours = (maxMinutes / 60).toFixed(2);

    return { totalHours, averageHours, maxHours };
  };

  const { totalHours, averageHours, maxHours } = calculateSummary();

  return (
    <div className="space-y-6 p-4">
      {/* Range Selector with Tabs */}
      <Tabs value={selectedRange} onValueChange={handleRangeChange} className="w-full">
        <TabsList className="grid w-full grid-cols-4">
          <TabsTrigger value="today">Hôm nay</TabsTrigger>
          <TabsTrigger value="7days">7 ngày</TabsTrigger>
          <TabsTrigger value="15days">15 ngày</TabsTrigger>
          <TabsTrigger value="30days">30 ngày</TabsTrigger>
        </TabsList>
      </Tabs>

      {/* Main Content */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        {/* Left Side: Circular Date Display */}
        <Card
          className="flex flex-col items-center justify-center cursor-pointer"
          onClick={() => setShowWeeklySettings(true)}
        >
          <CardContent className="flex flex-col items-center pt-6">
            <div className="w-40 h-40 flex items-center justify-center rounded-full border-4 border-primary">
              <span className="text-lg font-semibold text-center">
                {selectedRange === "today"
                  ? "Hôm nay"
                  : selectedRange === "7days"
                  ? "7 ngày qua"
                  : selectedRange === "15days"
                  ? "15 ngày qua"
                  : "30 ngày qua"}
              </span>
            </div>
            <span className="mt-2 text-sm text-muted-foreground">
              Nhấn để cấu hình giới hạn hàng tuần
            </span>
          </CardContent>
        </Card>

        {/* Right Side: Bar Chart */}
        <Card className="col-span-2">
          <CardHeader>
            <CardTitle>Thống kê sử dụng</CardTitle>
          </CardHeader>
          <CardContent className="pt-4">
            <div className="h-[400px]">
              <Bar
                data={generateChartData()}
                options={{
                  responsive: true,
                  maintainAspectRatio: false,
                  plugins: {
                    legend: {
                      display: false,
                    },
                    tooltip: {
                      backgroundColor: "rgba(0, 0, 0, 0.8)",
                      titleFont: { size: 14, family: "'Inter', sans-serif" },
                      bodyFont: { size: 12, family: "'Inter', sans-serif" },
                      padding: 10,
                      callbacks: {
                        label: (context) =>
                          `${context.dataset.label}: ${context.raw.toFixed(2)} ${
                            selectedRange === "today" ? "phút" : "giờ"
                          }`,
                      },
                    },
                  },
                  scales: {
                    x: {
                      title: {
                        display: true,
                        text: selectedRange === "today" ? "Giờ" : "Ngày",
                        font: { size: 14, family: "'Inter', sans-serif" },
                        color: "#1f2937",
                      },
                      ticks: {
                        font: { size: 12, family: "'Inter', sans-serif" },
                        color: "#4b5563",
                        maxRotation: 45,
                        minRotation: 45,
                      },
                      grid: { display: false },
                    },
                    y: {
                      title: {
                        display: true,
                        text: selectedRange === "today" ? "Phút sử dụng" : "Giờ sử dụng",
                        font: { size: 14, family: "'Inter', sans-serif" },
                        color: "#1f2937",
                      },
                      ticks: {
                        font: { size: 12, family: "'Inter', sans-serif" },
                        color: "#4b5563",
                        callback: (value) => `${value}${selectedRange === "today" ? "m" : "h"}`,
                      },
                      grid: {
                        color: "rgba(0, 0, 0, 0.05)",
                        drawBorder: false,
                      },
                      beginAtZero: true,
                    },
                  },
                  animation: {
                    duration: 1000,
                    easing: "easeOutQuart",
                  },
                }}
                aria-label="Biểu đồ thống kê thời gian sử dụng thiết bị"
              />
            </div>
          </CardContent>
        </Card>
      </div>

      {/* Summary Card */}
      <Card>
        <CardHeader>
          <CardTitle>Tóm tắt sử dụng</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <div className="flex flex-col items-center">
              <span className="text-lg font-bold">{totalHours} giờ</span>
              <span className="text-sm text-muted-foreground">Tổng thời gian sử dụng</span>
            </div>
            <div className="flex flex-col items-center">
              <span className="text-lg font-bold">{averageHours} giờ</span>
              <span className="text-sm text-muted-foreground">Thời gian trung bình hàng ngày</span>
            </div>
            <div className="flex flex-col items-center">
              <span className="text-lg font-bold">{maxHours} giờ</span>
              <span className="text-sm text-muted-foreground">Thời gian tối đa trong ngày</span>
            </div>
          </div>
        </CardContent>
      </Card>

      {/* Weekly Screen Time Limits Dialog */}
      <Dialog open={showWeeklySettings} onOpenChange={setShowWeeklySettings}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Cấu hình giới hạn thời gian sử dụng hàng tuần</DialogTitle>
          </DialogHeader>
          <div className="flex justify-center space-x-2 mb-4">
            {Object.keys(weeklyLimits).map((day) => (
              <Button
                key={day}
                variant={selectedDay === day ? "default" : "outline"}
                size="sm"
                className="w-10 h-10 rounded-full"
                onClick={() => handleDayClick(day)}
              >
                {day.slice(0, 2)}
              </Button>
            ))}
          </div>
          <Card>
            <CardContent className="pt-6 space-y-6">
              {/* Daily Limit */}
              <div className="grid grid-cols-2 gap-6">
                <div>
                  <Label htmlFor="hours">Giờ</Label>
                  <Input
                    id="hours"
                    type="number"
                    value={dailyLimit.hours}
                    onChange={(e) => handleDailyLimitChange("hours", parseInt(e.target.value, 10))}
                    min="0"
                    max="24"
                    className="text-center"
                  />
                </div>
                <div>
                  <Label htmlFor="minutes">Phút</Label>
                  <Input
                    id="minutes"
                    type="number"
                    value={Math.floor(dailyLimit.minutes)}
                    onChange={(e) => handleDailyLimitChange("minutes", parseInt(e.target.value, 10))}
                    min="0"
                    max="59"
                    className="text-center"
                  />
                </div>
              </div>
              {/* Allowed Times */}
              <div>
                <Label>Thời gian được phép</Label>
                <div className="space-y-4 mt-2 px-2">
                  {tempAllowedTimes.map((time, index) => (
                    <div key={index} className="flex items-center space-x-4 flex-wrap">
                      <Input
                        type="time"
                        value={time.split("-")[0]}
                        onChange={(e) => handleAllowedTimeChange(index, "start", e.target.value)}
                        className="w-32"
                      />
                      <span>-</span>
                      <Input
                        type="time"
                        value={time.split("-")[1]}
                        onChange={(e) => handleAllowedTimeChange(index, "end", e.target.value)}
                        className="w-32"
                      />
                      <Button
                        variant="destructive"
                        size="icon"
                        onClick={() => removeAllowedTime(index)}
                        aria-label="Xóa khoảng thời gian"
                      >
                        <Trash2 className="h-4 w-4" />
                      </Button>
                    </div>
                  ))}
                  <Button variant="outline" onClick={addAllowedTime} className="w-full">
                    Thêm mới
                  </Button>
                </div>
              </div>
            </CardContent>
          </Card>
          <DialogFooter>
            <Button variant="outline" onClick={() => setShowWeeklySettings(false)}>
              Hủy
            </Button>
            <Button onClick={saveDailyLimit}>
              Lưu
            </Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>

      {/* Notification Dialog */}
      <Dialog open={dialogOpen} onOpenChange={setDialogOpen}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>{dialogContent.title}</DialogTitle>
          </DialogHeader>
          <div className="py-4">
            <p>{dialogContent.message}</p>
          </div>
          <DialogFooter>
            <Button variant="outline" onClick={() => setDialogOpen(false)}>
              Đóng
            </Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>
    </div>
  );
}