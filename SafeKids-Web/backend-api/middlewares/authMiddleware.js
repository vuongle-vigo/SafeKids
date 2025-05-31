const jwt = require('jsonwebtoken');
const deviceModel = require('../models/deviceModel');

function verifyToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return res.status(401).json({ message: 'Missing or invalid token' });
    }
    const token = authHeader.split(' ')[1];
    jwt.verify(token, process.env.JWT_SECRET_KEY, (err, decoded) => {
        if (err) {
            return res.status(401).json({ message: 'Invalid token' });
        }
        req.user = decoded;  // Lưu thông tin user từ token vào req.user
        next();
    });
}

function verifyKidToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return res.status(401).json({ message: 'Missing or invalid token' });
    }
    const token = authHeader.split(' ')[1];
    jwt.verify(token, process.env.JWT_KID_SECRET_KEY, (err, decoded) => {
        if (err) {
            return res.status(401).json({ message: 'Invalid token' });
        }
        deviceModel.getDeviceByGuid(decoded.id, (err, device) => {
            if (err || !device) {
                return res.status(401).json({ message: 'Invalid GUID device' });
            }
            req.device = device;
            next();
        });
    });
}

module.exports = { 
    verifyToken,
    verifyKidToken
};
