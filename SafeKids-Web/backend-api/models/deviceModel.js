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
    const selectQuery = 'SELECT * FROM devices WHERE user_id = ?';
    const updateQuery = `
        UPDATE devices 
        SET device_status = 'offline' 
        WHERE user_id = ? 
          AND TIMESTAMPDIFF(MINUTE, last_activity, NOW()) >= 3
          AND device_status = 'online'
    `;

    connection.query(updateQuery, [userId], (updateErr) => {
        if (updateErr) {
            return callback(updateErr, null);
        }

        connection.query(selectQuery, [userId], (selectErr, results) => {
            if (selectErr) {
                return callback(selectErr, null);
            }

            callback(null, results);
        });
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

function deleteDevice(deviceId, callback) {
    const query = 'DELETE FROM devices WHERE device_id = ?';
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function updateDeviceName(deviceId, deviceName, callback) {
    const query = 'UPDATE devices SET device_name = ? WHERE device_id = ?';
    connection.query(query, [deviceName, deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function updateDeviceStatus(deviceId, callback) {
    //set last_activity to current time
    const now = new Date();
    const currentTimestamp = now.toLocaleString('sv-SE').replace('T', ' ').replace('Z', '');
    const query = 'UPDATE devices SET device_status = "online", last_activity = ? WHERE device_id = ?';
    connection.query(query, [currentTimestamp, deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

module.exports = {
    addDevice,
    getDeviceByUserId,
    uploadLastActivity,
    getDeviceById,
    getDeviceByGuid,
    deleteDevice,
    updateDeviceName,
    updateDeviceStatus
};
