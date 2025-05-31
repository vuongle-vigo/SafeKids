const processUsageModel = require('../models/processUsageModel');

function addProcessUsage(req, res) {
    const { processTitle, processPath, dateRecorded, startTime, timeUsage} = req.body;
    const deviceId = req.device.device_id; 
    processUsageModel.addProcessUsage(deviceId, processTitle, processPath, dateRecorded, startTime, timeUsage, (err, result) => {
        if (err) {
            console.error("Error adding process usage:", err);
            return res.status(500).json({ message: 'Error adding process usage' });
        }
        res.status(201).json(result);
    });
}

function getProcessUsageByDeviceAndDate(req, res) {
    const { dateRecorded } = req.body;
    const { deviceId } = req.params;
    processUsageModel.getProcessUsageByDeviceAndDate(deviceId, dateRecorded, (err, result) => {
        if (err) {
            console.error("Error fetching process usage:", err);
            return res.status(500).json({ message: 'Error fetching process usage' });
        }
        res.status(200).json(result);
    });
}

module.exports = {
    addProcessUsage,
    getProcessUsageByDeviceAndDate
};