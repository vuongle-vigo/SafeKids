const connection = require('../db');

function addPowerUsage(deviceId, date, hour, usage_minutes, callback) {
    // Check if a record for the given deviceId, date, and hour already exists
    const checkQuery = 'SELECT * FROM power_usage WHERE device_id = ? AND date = ? AND hour = ?';
    
    connection.query(checkQuery, [deviceId, date, hour], (err, results) => {
        if (err) {
            return callback({
                success: false,
                message: 'Error checking existing power usage.',
                error: err
            }, null);
        }

        if (results.length > 0) {
            // If an existing record is found, update it
            const updateQuery = 'UPDATE power_usage SET usage_minutes = ? WHERE device_id = ? AND date = ? AND hour = ?';
            connection.query(updateQuery, [usage_minutes, deviceId, date, hour], (err, result) => {
                if (err) {
                    return callback({
                        success: false,
                        message: 'Error updating power usage.',
                        error: err
                    }, null);
                }
                callback(null, {
                    success: true,
                    message: 'Power usage updated successfully.',
                    data: result
                });
            });
        } else {
            // If no existing record, insert a new one
            const insertQuery = 'INSERT INTO power_usage (device_id, date, hour, usage_minutes) VALUES (?, ?, ?, ?)';
            connection.query(insertQuery, [deviceId, date, hour, usage_minutes], (err, result) => {
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
    });
}

function getPowerUsage(deviceId, callback) {
    const query = 'SELECT * FROM power_usage WHERE device_id = ?';
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function getPowerUsageByTime(deviceId, startTime, endTime, callback) {
    console.log(startTime, endTime);
    const query = 'SELECT * FROM power_usage WHERE device_id = ? AND date BETWEEN ? AND ?';
    connection.query(query, [deviceId, startTime, endTime], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function deletePowerUsageByDeviceId(deviceId, callback) {
    const query = 'DELETE FROM power_usage WHERE device_id = ?';
    connection.query(query, [deviceId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

module.exports = {
    addPowerUsage,
    getPowerUsage,
    getPowerUsageByTime,
    deletePowerUsageByDeviceId
};