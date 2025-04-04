const processUsageModel = require('../models/processUsageModel');

function addProcessUsage(req, res) {
    const { processTitle, processPath, dateRecorded, startTime, timeUsage} = req.body;
    const deviceId = req.device.id; 
    processUsageModel.addProcessUsage(deviceId, processTitle, processPath, dateRecorded, startTime, timeUsage, (err, result) => {
        if (err) {
            console.error("Error adding process usage:", err);
            return res.status(500).json({ message: 'Error adding process usage' });
        }
        res.status(201).json(result);
    });
}

module.exports = {
    addProcessUsage
};