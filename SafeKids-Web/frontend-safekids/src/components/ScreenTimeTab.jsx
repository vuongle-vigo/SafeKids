import { useState } from "react";
import { Bar } from "react-chartjs-2";
import "chart.js/auto";

export default function ScreenTimeTab({ screenTimeLimit, usageData, onUpdate }) {
  const [selectedRange, setSelectedRange] = useState("today"); // Default range
  const [customRange, setCustomRange] = useState({ start: "", end: "" });
  const [showCustomPicker, setShowCustomPicker] = useState(false); // Toggle for custom picker
  const [showWeeklySettings, setShowWeeklySettings] = useState(false); // Toggle for weekly settings modal
  const [weeklyLimits, setWeeklyLimits] = useState({
    Monday: 4,
    Tuesday: 4,
    Wednesday: 4,
    Thursday: 4,
    Friday: 4,
    Saturday: 6,
    Sunday: 6,
  }); // Default limits for each day

  const handleRangeChange = (range) => {
    setSelectedRange(range);
    if (range === "custom") {
      setShowCustomPicker(true);
    } else {
      setShowCustomPicker(false);
      setCustomRange({ start: "", end: "" });
    }
    console.log(`Fetching data for range: ${range}`);
  };

  const handleCustomRangeChange = (field, value) => {
    setCustomRange((prev) => ({ ...prev, [field]: value }));
    if (customRange.start && customRange.end) {
      console.log(`Fetching data for custom range: ${customRange.start} to ${customRange.end}`);
    }
  };

  const handleWeeklyLimitChange = (day, value) => {
    setWeeklyLimits((prev) => ({ ...prev, [day]: value }));
    console.log(`Updated ${day} limit to ${value} hours`);
  };
  const generateChartData = () => {
    if (selectedRange === "today") {
      return {
        labels: Array.from({ length: 24 }, (_, i) => `${i}:00`),
        datasets: [
          {
            label: "Minutes Used",
            data: Array.from({ length: 24 }, (_, i) =>
              usageData
                .filter((data) => data.hour === i)
                .reduce((sum, data) => sum + data.usage_minutes, 0)
            ),
            backgroundColor: "rgba(54, 162, 235, 0.6)",
          },
        ],
      };
    } else {
      const days = selectedRange === "7days" ? 7 : selectedRange === "15days" ? 15 : 30;
      const dateLabels = Array.from({ length: days }, (_, i) => {
        const date = new Date();
        date.setDate(date.getDate() - i);
        return date.toISOString().split("T")[0];
      }).reverse();

      return {
        labels: dateLabels,
        datasets: [
          {
            label: "Hours Used",
            data: dateLabels.map((date) =>
              usageData
                .filter((data) => data.date.startsWith(date))
                .reduce((sum, data) => sum + data.usage_minutes / 60, 0)
            ),
            backgroundColor: "rgba(75, 192, 192, 0.6)",
          },
        ],
      };
    }
  };

  const calculateSummary = () => {
    const totalMinutes = usageData.reduce((sum, data) => sum + data.usage_minutes, 0);
    const totalHours = (totalMinutes / 60).toFixed(2);
    const averageMinutes =
      usageData.length > 0 ? (totalMinutes / usageData.length).toFixed(2) : 0;
    const maxMinutes = Math.max(...usageData.map((data) => data.usage_minutes));
    const maxHours = (maxMinutes / 60).toFixed(2);
    return { totalHours, averageMinutes, maxHours };
  };

  const { totalHours, averageHours, maxHours } = calculateSummary();

  return (
    <div>
      {/* Range Selector */}
      <div className="flex justify-center space-x-2 mb-4 relative">
        {["today", "7days", "15days", "30days", "custom"].map((range) => (
          <button
            key={range}
            onClick={() => handleRangeChange(range)}
            className={`px-3 py-1 text-sm rounded ${
              selectedRange === range
                ? "bg-blue-500 text-white"
                : "bg-gray-200 text-gray-700 hover:bg-gray-300"
            }`}
          >
            {range === "today"
              ? "Today"
              : range === "7days"
              ? "7 Days"
              : range === "15days"
              ? "15 Days"
              : range === "30days"
              ? "30 Days"
              : "Custom"}
          </button>
        ))}

        {/* Custom Date Range Picker */}
        {showCustomPicker && (
          <div className="absolute top-12 bg-white border rounded shadow-lg p-4 z-10">
            <div className="flex space-x-4">
              <div>
                <label className="block text-sm font-medium text-gray-700">Start Date</label>
                <input
                  type="date"
                  value={customRange.start}
                  onChange={(e) => handleCustomRangeChange("start", e.target.value)}
                  className="border rounded px-2 py-1 text-sm"
                />
              </div>
              <div>
                <label className="block text-sm font-medium text-gray-700">End Date</label>
                <input
                  type="date"
                  value={customRange.end}
                  onChange={(e) => handleCustomRangeChange("end", e.target.value)}
                  className="border rounded px-2 py-1 text-sm"
                />
              </div>
            </div>
            <div className="mt-4 flex justify-end">
              <button
                onClick={() => setShowCustomPicker(false)}
                className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600"
              >
                Apply
              </button>
            </div>
          </div>
        )}
      </div>

      <div className="flex space-x-4">
        {/* Left Side: Circular Date Display */}
        <div
          className="w-1/3 flex flex-col items-center justify-center border rounded p-4 bg-gray-50 cursor-pointer"
          onClick={() => setShowWeeklySettings(true)} // Open modal on click
        >
          <div className="w-40 h-40 flex items-center justify-center rounded-full border-4 border-blue-500">
            <span className="text-lg font-semibold">
              {selectedRange === "custom"
                ? `${customRange.start || "Start"} - ${customRange.end || "End"}`
                : selectedRange === "today"
                ? "Today"
                : selectedRange === "7days"
                ? "Last 7 Days"
                : selectedRange === "15days"
                ? "Last 15 Days"
                : "Last 30 Days"}
            </span>
          </div>
          <span className="mt-2 text-sm text-gray-500">Click to configure weekly limits</span>
        </div>

        {/* Right Side: Bar Chart */}
        <div className="w-2/3 border rounded p-4 bg-gray-50">
          <Bar
            data={generateChartData()}
            options={{
              responsive: true,
              plugins: {
                legend: {
                  display: false,
                },
              },
              scales: {
                x: {
                  title: {
                    display: true,
                    text: selectedRange === "today" ? "Hours" : "Days",
                  },
                },
                y: {
                  title: {
                    display: true,
                    text: selectedRange === "today" ? "Minutes Used" : "Hours Used",
                  },
                  beginAtZero: true,
                },
              },
            }}
          />
        </div>
      </div>

  

      {/* Summary Card */}
      <div className="mt-6 border rounded p-4 bg-gray-50">
        <h3 className="text-md font-semibold mb-4">Summary</h3>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div className="flex flex-col items-center">
            <span className="text-lg font-bold">{totalHours} hrs</span>
            <span className="text-sm text-gray-500">Total Screen Time</span>
          </div>
          <div className="flex flex-col items-center">
            <span className="text-lg font-bold">{averageHours} hrs</span>
            <span className="text-sm text-gray-500">Average Daily Usage</span>
          </div>
          <div className="flex flex-col items-center">
            <span className="text-lg font-bold">{maxHours} hrs</span>
            <span className="text-sm text-gray-500">Max Usage in a Day</span>
          </div>
        </div>
      </div>

      {/* Weekly Screen Time Limits Modal */}
      {showWeeklySettings && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-20">
          <div className="bg-white rounded-lg shadow-lg p-6 w-96">
            <h3 className="text-lg font-semibold mb-4 text-center">Configure Weekly Screen Time Limits</h3>
            <div className="grid grid-cols-1 gap-4">
              {Object.keys(weeklyLimits).map((day) => (
                <div key={day} className="flex items-center justify-between">
                  <span className="text-sm font-medium w-24 text-left">{day}</span> {/* Fixed width for alignment */}
                  <input
                    type="number"
                    min="0"
                    max="24"
                    value={weeklyLimits[day]}
                    onChange={(e) => handleWeeklyLimitChange(day, Math.min(24, e.target.value))} // Limit to 24
                    className="border rounded px-2 py-1 text-sm w-16 text-center"
                  />
                  <span className="text-sm text-gray-500 ml-2">hrs</span>
                </div>
              ))}
            </div>
            <div className="mt-6 flex justify-end space-x-4">
              <button
                onClick={() => setShowWeeklySettings(false)}
                className="px-4 py-2 bg-gray-300 text-gray-700 rounded hover:bg-gray-400"
              >
                Cancel
              </button>
              <button
                onClick={() => setShowWeeklySettings(false)}
                className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600"
              >
                Save
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
