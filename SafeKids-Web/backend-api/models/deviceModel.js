const connection = require('../db');

function addDevice(deviceId, deviceName, userId, callback) {
    console.log(deviceId, deviceName, userId)
    const query = 'INSERT INTO devices (device_guid, device_name, user_id) VALUES (?, ?, ?)';
    connection.query(query, [deviceId, deviceName, userId], (err, result) => {
        if (err) {
            console.error('Error inserting device:', err);
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

function getDeviceById(deviceId, callback) {
    const query = 'SELECT * FROM devices WHERE device_id = ?';
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results[0]); // Assuming device_id is unique
    });
}

function getDeviceByGuid(deviceGuid, callback) {
    const query = 'SELECT device_id FROM devices WHERE device_guid = ?';
    connection.query(query, [deviceGuid], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results[0]); // Assuming device_guid is unique
    });
}

module.exports = {
    addDevice,
    getDeviceByUserId,
    uploadLastActivity,
    getDeviceById,
    getDeviceByGuid,
};