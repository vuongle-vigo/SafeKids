const connection = require('../db');

function getTimeUsage(deviceId) {
    return new Promise((resolve, reject) => {
        const query = 'SELECT * FROM time_usage WHERE device_id = ?';
        connection.query(query, [deviceId], (err, results) => {
            if (err) {
                return reject(err);
            }
            resolve(results);
        });
    });
}

function getTimeLimit(deviceId) {
    return new Promise((resolve, reject) => {
        const query = 'SELECT * FROM time_limits WHERE device_id = ?';
        connection.query(query, [deviceId], (err, results) => {
            if (err) {
                return reject(err);
            }
            resolve(results[0]); // Assuming one limit per device
        });
    });
}

module.exports = {
    getTimeUsage,
    getTimeLimit,
};
