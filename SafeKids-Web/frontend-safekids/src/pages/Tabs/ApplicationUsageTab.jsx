import React, { useEffect, useState } from "react";
import axios from "axios";

export default function ApplicationUsageTab({ deviceId, selectedDate, onDateChange, isActive }) {
  const [usageData, setUsageData] = useState([]);

  useEffect(() => {
    if (isActive && selectedDate && deviceId) {
      fetchUsageData(deviceId, selectedDate);
    }
  }, [isActive, deviceId, selectedDate]);

  const fetchUsageData = async (deviceId, date) => {
    try {
      const token = localStorage.getItem("token");
      const response = await axios.post(
        `/api/devices/${deviceId}/process-usage`,
        { dateRecorded: date },
        {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );
      setUsageData(response.data.data || []);
    } catch (error) {
      console.error("Error fetching process usage data:", error);
      setUsageData([]);
    }
  };

  return (
    <div>
      <div className="mb-4">
        <label htmlFor="date" className="block text-sm font-medium text-gray-700">
          Select Date:
        </label>
        <input
          type="date"
          id="date"
          value={selectedDate}
          onChange={(e) => onDateChange(e.target.value)}
          className="mt-1 block w-full border-gray-300 rounded-md shadow-sm focus:ring-blue-500 focus:border-blue-500 sm:text-sm"
        />
      </div>
      <div className="timeline-container overflow-y-auto">
        <div className="timeline flex flex-col items-start space-y-6">
          {usageData.length > 0 ? (
            usageData.map((entry, index) => (
              <div key={index} className="timeline-node relative flex items-center space-x-4">
                <div
                  className={`node px-4 py-2 flex items-center justify-between rounded-full ${
                    entry.time_usage > 120 ? "bg-red-500" : "bg-blue-500"
                  } text-white`}
                  title={`${entry.process_title} - ${entry.time_usage} mins`}
                >
                  <span className="font-medium">{entry.process_title}</span>
                  <span className="ml-4 text-sm">{entry.time_usage} mins</span>
                </div>
                <div className="tooltip absolute left-20 bg-gray-800 text-white text-xs rounded-md p-2 hidden group-hover:block">
                  <p>{entry.process_title}</p>
                  <p>Start: {entry.start_time}</p>
                  <p>Usage: {entry.time_usage} mins</p>
                </div>
                <span className="text-xs text-gray-500 ml-4">{entry.start_time}</span>
              </div>
            ))
          ) : (
            <p className="text-gray-500">No usage data available for the selected date.</p>
          )}
        </div>
      </div>
    </div>
  );
}
