const mysql = require('mysql2');
require('dotenv').config();

const connection = mysql.createConnection(
    {
        host: process.env.DB_HOST,
        user: process.env.DB_USER,
        password: process.env.DB_PASSWORD,
        database: process.env.DB_NAME,
        dateStrings: true,
    }
);

connection.connect((err) => {
    if (err) {
        console.error('Error connect MySQL: ' + err.stack);
        return;
    }

    console.log('Connect MYSQL success with id: ' + connection.threadId);
});

module.exports = connection;