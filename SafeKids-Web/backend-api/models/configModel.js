const connection = require('../db');

function getConfig(deviceId, callback) {
    const query = 'SELECT * FROM configs WHERE device_id = ?';
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function updateTimelimitConfig(deviceId, configData, callback) {
    const query = 'UPDATE configs SET time_limit_daily = ? WHERE device_id = ?';
    connection.query(query, [JSON.stringify(configData), deviceId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

function updateAppConfig(deviceId, configData, callback) {
    const query = 'UPDATE configs SET config_apps = ? WHERE device_id = ?';
    connection.query(query, [JSON.stringify(configData), deviceId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

module.exports = {
    getConfig,
    updateTimelimitConfig,
    updateAppConfig
};
