const connection = require('../db');

function addProcessUsage(deviceId, processTitle, processPath, dateRecorded, startTime, timeUsage, callback) {
    const query = `
        INSERT INTO process_usage (device_id, process_title, process_path, date_recorded, start_time, time_usage)
        VALUES (?, ?, ?, ?, ?, ?)
        ON DUPLICATE KEY UPDATE 
            device_id = VALUES(device_id),
            process_path = VALUES(process_path),
            time_usage = VALUES(time_usage)
    `;
    connection.query(query, [deviceId, processTitle, processPath, dateRecorded, startTime, timeUsage], (err, result) => {
        if (err) {
            return callback({
                success: false,
                message: 'Error inserting or updating process usage.',
                error: err
            }, null);
        }
        callback(null, {
            success: true,
            message: result.affectedRows === 1 ? 'Process usage inserted successfully.' : 'Process usage updated successfully.',
            data: result
        });
    });
}


function getProcessUsageByDeviceAndDate(deviceId, dateRecorded, callback) {
    const query = 'SELECT * FROM process_usage WHERE device_id = ? AND date_recorded = ?';
    connection.query(query, [deviceId, dateRecorded], (err, results) => {
        if (err) {
            return callback({
                success: false,
                message: 'Error fetching process usage.',
                error: err
            }, null);
        }
        callback(null, {
            success: true,
            message: 'Process usage fetched successfully.',
            data: results
        });
    });
}

module.exports = {
    addProcessUsage,
    getProcessUsageByDeviceAndDate
};