require('dotenv').config();

const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');
const userModel = require('../models/userModel');
const deviceModel = require('../models/deviceModel');

function register(req, res) {
    const { username, password, email } = req.body;
    
    bcrypt.hash(password, 10, (err, hashedPassword) => {
        if (err) {
            return res.status(500).json({ message: 'Error hashing password' });
        }

        userModel.registryUser(username, hashedPassword, email, (err, result) => {
            if (err) {
                return res.status(500).json({ message: 'Error registry user' });
            }

            res.status(201).json({ message: 'Registry user success' });
        });
    })
}

function login(req, res) {
    // Lấy thông tin Basic Auth từ header
    const authHeader = req.headers['authorization'];
    
    if (!authHeader || !authHeader.startsWith('Basic ')) {
        return res.status(400).json({ message: 'Missing or invalid Authorization header' });
    }

    // Tách ra phần base64 của username:password
    const base64Credentials = authHeader.split(' ')[1];
    const credentials = Buffer.from(base64Credentials, 'base64').toString('utf8');
    
    // Tách username và password
    const [username, password] = credentials.split(':');
    
    // Kiểm tra nếu không có username hoặc password
    if (!username || !password) {
        return res.status(400).json({ message: 'Invalid credentials format' });
    }

    // Kiểm tra username trong database
    userModel.findUserByUsername(username, (err, user) => {
        if (err) {
            return res.status(500).json({ message: 'Error checking username' });
        }

        if (!user) {
            return res.status(404).json({ message: 'Username not found' });
        }

        // Kiểm tra mật khẩu
        bcrypt.compare(password, user.password, (err, isMatch) => {
            if (err) {
                return res.status(500).json({ message: 'Error comparing password' });
            }

            if (!isMatch) {
                return res.status(401).json({ message: 'Incorrect password' });
            }

            // Tạo JWT token
            const token = jwt.sign(
                { id: user.id, username: user.username },
                process.env.JWT_SECRET_KEY,
                { expiresIn: '1h' }
            );

            res.json({ message: 'Login successful', token });
        });
    });
}

function deviceLogin(req, res) {
    const { deviceId, deviceName } = req.body;

    const authHeader = req.headers['authorization'];
    
    if (!authHeader || !authHeader.startsWith('Basic ')) {
        return res.status(400).json({ message: 'Missing or invalid Authorization header' });
    }

    const base64Credentials = authHeader.split(' ')[1];
    const credentials = Buffer.from(base64Credentials, 'base64').toString('utf8');
    
    const [username, password] = credentials.split(':');
    
    if (!username || !password) {
        return res.status(400).json({ message: 'Invalid credentials format' });
    }

    // Kiểm tra username trong database
    userModel.findUserByUsername(username, (err, user) => {
        if (err) {
            return res.status(500).json({ message: 'Error checking username' });
        }

        if (!user) {
            return res.status(404).json({ message: 'Username not found' });
        }

        // Kiểm tra mật khẩu
        bcrypt.compare(password, user.password, (err, isMatch) => {
            if (err) {
                return res.status(500).json({ message: 'Error comparing password' });
            }

            if (!isMatch) {
                return res.status(401).json({ message: 'Incorrect password' });
            }

            // Generate device token
            const deviceToken = jwt.sign(
                { id: user.id, username: user.username, device: true },
                process.env.JWT_SECRET_KEY,
                { expiresIn: '365d' } // Token valid for 1 year
            );

            // Add device to database
            deviceModel.addDevice(deviceId, deviceName, user.id, (err, result) => {
                if (err) {
                    return res.status(500).json({ message: 'Error adding device' });
                }

                res.json({ message: 'Device login successful', token: deviceToken });
            });
        });
    });
}

module.exports = {
    register,
    login,
    deviceLogin
};