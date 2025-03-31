import React from "react";

export default function ActivitysTab({ activeActivities }) {
  return (
    <div>
      <h2 className="text-lg font-semibold mb-4">Current Activities</h2>
      {activeActivities.length > 0 ? (
        <ul className="list-disc pl-5">
          {activeActivities.map((activity, index) => (
            <li key={index} className="mb-2">
              <span>{activity}</span>
            </li>
          ))}
        </ul>
      ) : (
        <p className="text-gray-500">No active activities at the moment.</p>
      )}
    </div>
  );
}
