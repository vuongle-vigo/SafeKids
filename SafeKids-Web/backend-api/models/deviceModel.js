const connection = require('../db');

function addDevice(deviceId, deviceName, userId, callback) {
    console.log(deviceId, deviceName, userId);

    // Đối tượng JSON cho time_limit_daily
    const timeLimitDaily = {
        friday: { max_hours: 7, allowed_time: ["09:00-12:00", "14:00-20:00"] },
        monday: { max_hours: 6, allowed_time: ["14:00-18:00"] },
        sunday: { max_hours: 3, allowed_time: ["10:00-13:00", "15:00-18:00"] },
        tuesday: { max_hours: 5, allowed_time: ["09:00-13:00", "15:00-19:00"] },
        saturday: { max_hours: 4, allowed_time: ["10:00-14:00", "16:00-20:00"] },
        thursday: { max_hours: 6, allowed_time: ["08:00-12:00", "14:00-18:00"] },
        wednesday: { max_hours: 5, allowed_time: ["08:00-11:00", "13:00-17:00"] }
    };

    // Đối tượng JSON cho config_apps
    const configApps = { blocked: [], uninstall: [] };

    // Bắt đầu transaction
    connection.beginTransaction((err) => {
        if (err) {
            console.error('Error starting transaction:', err);
            return callback(err, null);
        }

        // Thêm bản ghi vào bảng devices
        const deviceQuery = 'INSERT INTO devices (device_guid, device_name, user_id, device_status, last_activity) VALUES (?, ?, ?, ?, NOW())';
        connection.query(deviceQuery, [deviceId, deviceName, userId, 'online'], (err, deviceResult) => {
            if (err) {
                console.error('Error inserting device:', err);
                return connection.rollback(() => callback(err, null));
            }

            // Lấy device_id của bản ghi vừa thêm
            const deviceIdInserted = deviceResult.insertId;

            // Thêm bản ghi vào bảng configs với time_limit_daily và config_apps
            const configQuery = 'INSERT INTO configs (device_id, time_limit_daily, config_apps) VALUES (?, ?, ?)';
            connection.query(configQuery, [deviceIdInserted, JSON.stringify(timeLimitDaily), JSON.stringify(configApps)], (err, configResult) => {
                if (err) {
                    console.error('Error inserting config:', err);
                    return connection.rollback(() => callback(err, null));
                }

                // Commit transaction
                connection.commit((err) => {
                    if (err) {
                        console.error('Error committing transaction:', err);
                        return connection.rollback(() => callback(err, null));
                    }

                    // Trả về kết quả
                    callback(null, { deviceResult, configResult });
                });
            });
        });
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
