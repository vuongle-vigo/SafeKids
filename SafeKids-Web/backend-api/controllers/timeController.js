const timeModel = require('../models/timeModel');

async function getScreenTime(req, res) {
    const { deviceId } = req.params;
    try {
        const usage = await timeModel.getTimeUsage(deviceId);
        const limit = await timeModel.getTimeLimit(deviceId);
        res.status(200).json({ usage, limit });
    } catch (error) {
        console.error("Error fetching screen time data:", error);
        res.status(500).json({ message: "Error fetching screen time data" });
    }
}

module.exports = {
    getScreenTime,
};
