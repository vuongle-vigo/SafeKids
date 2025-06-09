const connection = require('../db');

function createUser(email, password, callback) {
    const query = 'INSERT INTO users (email, password) VALUES (?, ?)';
    connection.query(query, [email, password], (err, result) => {
        if (err) {
            return callback(err, null);
        }

        callback(null, result);
    });
}

function findUserByEmail(email, callback) {
    const query = 'SELECT * FROM users WHERE email = ?';
    connection.query(query, [email], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result[0]);
    });
}

function getAllUsers(callback) {
    const query = 'SELECT * FROM users';
    connection.query(query, (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

function deleteUser(id, callback) {
    const query = 'DELETE FROM users WHERE user_id = ?';
    connection.query(query, [id], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

function changePassword(userId, newPassword, callback) {
    const query = 'UPDATE users SET password = ? WHERE user_id = ?';
    connection.query(query, [newPassword, userId], (err, result) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, result);
    });
}

module.exports = {
    createUser,
    findUserByEmail,
    getAllUsers,
    deleteUser, 
    changePassword
};
