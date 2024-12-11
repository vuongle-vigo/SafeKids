require('dotenv').config();

const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');
const userModel = require('../models/userModel');


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
    const { username, password } = req.body;

    userModel.findUserByUsername(username, (err, user) => {
        if (err) {
            return res.status(500).json({ message: 'Error check username' });
        }

        if (!user) {
            return res.status(404).json({ message: 'Username not exists' });
        }

        bcrypt.compare(password, user.password, (err, isMatch) => {
            if (err) {
                return res.status(500).json({ message: 'Error compare password' });
            }

            if (!isMatch) {
                return res.status(401).json({ message: 'Password incorrect' });
            }

            const token = jwt.sign(
                { id: user.id, username: user.username },
                process.env.JWT_SECRET_KEY,
                { expiresIn: '1h' }
            );

            res.json({ message: 'Login successfully', token });
        });
    });
}

module.exports = {
    register, 
    login,
}