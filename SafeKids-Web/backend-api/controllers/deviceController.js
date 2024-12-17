require('dotenv').config();

const deviceModel = require('../models/deviceModel');

function devices(req, res) {
    const userId = req.user.id;
    deviceModel.getDeviceByUserId(userId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching devices' });
        }
        res.status(200).json(result);
    });
}

module.exports={
    devices,
}
