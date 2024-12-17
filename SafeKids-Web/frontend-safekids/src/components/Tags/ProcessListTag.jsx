import { useState, useEffect } from 'react';
import axios from 'axios';

export default function ProcessListTag({ deviceId }) {
  const [processes, setProcesses] = useState([]);

  useEffect(() => {
    const fetchProcesses = async () => {
      try {
        const response = await axios.get(`/api/devices/${deviceId}/processes`);
        setProcesses(response.data);
      } catch (error) {
        console.error('Error fetching processes:', error);
      }
    };

    fetchProcesses();
  }, [deviceId]);

  return (
    <div className="tag">
      <h3>Running Processes</h3>
      {processes.length > 0 ? (
        <ul>
          {processes.map((process) => (
            <li key={process.id}>{process.name}</li>
          ))}
        </ul>
      ) : (
        <p>No processes running.</p>
      )}
    </div>
  );
}
