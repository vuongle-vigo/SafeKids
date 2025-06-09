const powerUsageModel = require('../models/powerUsageModel');

function addPowerUsage(req, res) {
    const deviceId = req.device.device_id;
    const promises = req.body.map((item) => {
        const { date, hour, usage_minutes } = item;
        return new Promise((resolve, reject) => {
            powerUsageModel.addPowerUsage(deviceId, date, hour, usage_minutes, (err, result) => {
                if (err) {
                    // console.error("Error adding power usage:", err);
                    reject(err); 
                }
                resolve(result);  
            });
        });
    });

    Promise.all(promises)
        .then(() => {
            res.status(201).json({ message: 'Power usage added successfully' });
        })
        .catch((err) => {
            res.status(500).json({ message: 'Error adding power usage' });
        });
}


function getPowerUsage(req, res) {
    powerUsageModel.getPowerUsage(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching power usage' });
        }
        res.status(200).json(result);
    });
}

function getPowerUsageByTime(req, res) {
    const { startTime, endTime } = req.body;
    const { deviceId } = req.params;
    powerUsageModel.getPowerUsageByTime(deviceId, startTime, endTime, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching power usage' });
        }
        res.status(200).json(result);
    });
}

function deletePowerUsageByDeviceId(req, res) {
    const { deviceId } = req.params;
    powerUsageModel.deletePowerUsageByDeviceId(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error deleting power usage' });
        }
        res.status(200).json({ message: 'Power usage deleted successfully' });
    });
}

module.exports = {
    addPowerUsage,
    getPowerUsage,
    getPowerUsageByTime,
    deletePowerUsageByDeviceId
};