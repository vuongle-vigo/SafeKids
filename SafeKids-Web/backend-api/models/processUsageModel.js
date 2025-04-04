const connection = require('../db');

function addProcessUsage(deviceId, processTitle, processPath, dateRecorded, startTime, timeUsage, callback) {
    const query = 'INSERT INTO process_usage (device_id, process_title, process_path, date_recorded, start_time, time_usage) VALUES (?, ?, ?, ?, ?, ?)';
    connection.query(query, [deviceId, processTitle, processPath, dateRecorded, startTime, timeUsage], (err, result) => {
        if (err) {
            return callback({
                success: false,
                message: 'Error inserting process usage.',
                error: err
            }, null);
        }
        callback(null, {
            success: true,
            message: 'Process usage inserted successfully.',
            data: result
        });
    });
}

module.exports = {
    addProcessUsage
};