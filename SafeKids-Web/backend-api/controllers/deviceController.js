require('dotenv').config();

const deviceModel = require('../models/deviceModel');

function devices(req, res) {
    const userId = req.user.id;
    console.log('User ID:', userId);
    deviceModel.getDeviceByUserId(userId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching devices' });
        }
        res.status(200).json(result);
    });
}

function getDeviceById(req, res) {
    const deviceId = req.params.deviceId;
    deviceModel.getDeviceById(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching device' });
        }
        res.status(200).json(result);
    });
}

module.exports={
    devices,
    getDeviceById
}
