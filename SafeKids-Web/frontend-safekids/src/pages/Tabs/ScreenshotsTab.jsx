export default function ScreenshotsTab({ screenshots }) {
  return (
    <div>
      <h2 className="text-lg font-semibold mb-4">Screenshots</h2>
      {screenshots.length > 0 ? (
        <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
          {screenshots.map((screenshot, index) => (
            <img
              key={index}
              src={screenshot.url}
              alt={`Screenshot ${index + 1}`}
              className="border rounded shadow"
            />
          ))}
        </div>
      ) : (
        <p className="text-gray-500">No screenshots available.</p>
      )}
    </div>
  );
}
