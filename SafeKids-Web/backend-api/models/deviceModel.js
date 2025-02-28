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
    console.log(userId);
    const query = 'SELECT * FROM devices WHERE user_id = ?';
    connection.query(query, [userId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        console.log('results:', results);
        callback(null, results);
    });
}

function uploadLastActivity(deviceId, callback) {
    const currentTimestamp = new Date().toISOString().slice(0, 19).replace('T', ' ');
    const query = `UPDATE devices SET last_activity = ? WHERE device_id = ?`;   

    connection.execute(query, [currentTimestamp, deviceId], (err, results) => {
        if (err) {
          console.error('Error updating timestamp:', err);
          return;
        }

        console.log('Timestamp updated successfully');
      });
}

module.exports = {
    addDevice,
    getDeviceByUserId,
    uploadLastActivity
};