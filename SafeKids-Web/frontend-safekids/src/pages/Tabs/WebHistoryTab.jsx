import { useState, useEffect } from "react";
import axios from "axios";

export default function WebHistoryTab({ deviceId }) {
  const [webHistory, setWebHistory] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchWebHistory = async () => {
      try {
        const token = localStorage.getItem("token");
        const response = await axios.get(`/api/devices/${deviceId}/web-history`, {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        });
        setWebHistory(response.data);
      } catch (error) {
        console.error("Error fetching web history:", error);
      } finally {
        setLoading(false);
      }
    };

    fetchWebHistory();
  }, [deviceId]);

  if (loading) {
    return <p className="text-gray-500">Loading web history...</p>;
  }

  if (webHistory.length === 0) {
    return <p className="text-gray-500">No web history available.</p>;
  }

  return (
    <div>
      <h2 className="text-lg font-semibold mb-4">Web History</h2>
      <ul className="space-y-2">
        {webHistory.map((entry, index) => (
          <li key={index} className="border-b pb-2">
            <p className="text-sm">
              <span className="font-medium">URL:</span> {entry.url}
            </p>
            <p className="text-sm">
              <span className="font-medium">Visited At:</span> {new Date(entry.timestamp).toLocaleString()}
            </p>
          </li>
        ))}
      </ul>
    </div>
  );
}
