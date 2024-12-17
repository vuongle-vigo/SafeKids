import { useState } from 'react';
import axios from 'axios';

export default function ScreenshotTag({ deviceId }) {
  const [screenshot, setScreenshot] = useState(null);

  const captureScreenshot = async () => {
    try {
      const response = await axios.post(`/api/devices/${deviceId}/screenshot`);
      setScreenshot(response.data.imageUrl);
    } catch (error) {
      console.error('Error capturing screenshot:', error);
    }
  };

  return (
    <div className="tag">
      <h3>Screenshot</h3>
      <button onClick={captureScreenshot}>Capture Screenshot</button>
      {screenshot && <img src={screenshot} alt="Screenshot" />}
    </div>
  );
}
