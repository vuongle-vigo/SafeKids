const connection = require('../db');

function addMachineUsage(deviceId, date, hour, usage_minutes, callback) {
    const query = 'INSERT INTO power_usage (device_id, date, hour, usage_minutes) VALUES (?, ?, ?, ?)';
    connection.query(query, [deviceId, date, hour, usage_minutes], (err, result) => {
        if (err) {
            return callback({
                success: false,
                message: 'Error inserting power usage.',
                error: err
            }, null);
        }
        callback(null, {
            success: true,
            message: 'Power usage inserted successfully.',
            data: result
        });
    });
}

function getMachineUsage(deviceId, callback) {
    const query = 'SELECT * FROM power_usage WHERE device_id = ?';
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function getMachineUsageByTime(deviceId, startTime, endTime, callback) {
    const query = 'SELECT * FROM power_usage WHERE device_id = ? AND date BETWEEN ? AND ?';
    connection.query(query, [deviceId, startTime, endTime], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

module.exports = {
    addMachineUsage,
    getMachineUsage,
    getMachineUsageByTime
};