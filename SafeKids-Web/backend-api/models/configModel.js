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

function updateCommandConfig(deviceId, configData, callback) {
    console.log("Updating command config for device:", deviceId, "with data:", configData);
    const query = 'UPDATE configs SET command = ? WHERE device_id = ?';
    connection.query(query, [JSON.stringify(configData), deviceId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

function getCommandConfigDevice(deviceId, callback) {
    const query = 'SELECT device_id, command FROM configs';
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function deleteConfigByDeviceId(deviceId, callback) {
    const query = 'DELETE FROM configs WHERE device_id = ?';
    connection.query(query, [deviceId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

module.exports = {
    getConfig,
    updateTimelimitConfig,
    updateAppConfig,
    updateCommandConfig,
    getCommandConfigDevice, 
    deleteConfigByDeviceId
};
