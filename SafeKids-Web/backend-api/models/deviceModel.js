const connection = require('../db');

function addDevice(deviceId, deviceName, userId, callback) {
    const query = 'INSERT INTO devices (device_id, device_name, user_id) VALUES (?, ?, ?)';
    connection.query(query, [deviceId, deviceName, userId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

function getDeviceByUserId(userId, callback) {
    const query = 'SELECT * FROM devices WHERE user_id = ?';
    connection.query(query, [userId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

module.exports = {
    addDevice,
    getDeviceByUserId,
};