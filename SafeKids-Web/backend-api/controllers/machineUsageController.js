const machineUsageModel = require('../models/machineUsageModel');

function addMachineUsage(req, res) {
    const { date, hour, usage_minutes} = req.body;
    const deviceId = req.device.id;
    machineUsageModel.addMachineUsage(deviceId, date, hour, usage_minutes, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error adding power usage' });
        }
        res.status(201).json(result);
    });
}

function getMachineUsage(req, res) {
    machineUsageModel.getMachineUsage(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching power usage' });
        }
        res.status(200).json(result);
    });
}

function getMachineUsageByTime(req, res) {
    const { startTime, endTime } = req.body;
    const { deviceId } = req.params;
    machineUsageModel.getMachineUsageByTime(deviceId, startTime, endTime, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching power usage' });
        }
        res.status(200).json(result);
    });
}

module.exports = {
    addMachineUsage,
    getMachineUsage,
    getMachineUsageByTime
};