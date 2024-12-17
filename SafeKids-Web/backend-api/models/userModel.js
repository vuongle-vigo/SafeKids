const connection = require('../db');
const { connect } = require('../routes');

function registryUser(username, password, email, callback) {
    const query = 'INSERT INTO users (username, password, email) VALUES (?, ?, ?)';
    connection.query(query, [username, password, email], (err, result) => {
        if (err) {
            return callback(err, null);
        }

        callback(null, result);
    });
}

function findUserByUsername(username, callback) {
    const query = 'SELECT * FROM users WHERE username = ?';
    connection.query(query, [username], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result[0]);
    });
}

module.exports = {
    registryUser,
    findUserByUsername,
};
