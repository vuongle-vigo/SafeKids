import { useState, useEffect } from 'react';
import axios from 'axios';

export default function TimeUsageTag({ deviceId }) {
  const [usageTime, setUsageTime] = useState(null);

  useEffect(() => {
    const fetchUsageTime = async () => {
      try {
        const response = await axios.get(`/api/devices/${deviceId}/usage-time`);
        setUsageTime(response.data);
      } catch (error) {
        console.error('Error fetching usage time:', error);
      }
    };

    fetchUsageTime();
  }, [deviceId]);

  return (
    <div className="tag">
      <h3>Time Usage</h3>
      {usageTime ? (
        <p>Device has been used for {usageTime} hours today.</p>
      ) : (
        <p>Loading time usage...</p>
      )}
    </div>
  );
}
