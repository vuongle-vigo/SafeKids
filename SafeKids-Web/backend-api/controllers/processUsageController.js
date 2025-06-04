const processUsageModel = require('../models/processUsageModel');

function addProcessUsage(req, res) {
    const processUsages = Array.isArray(req.body) ? req.body : [req.body]; // Đảm bảo req.body là mảng
    const deviceId = req.device.device_id;
    const results = [];
    let errorOccurred = false;

    // Filter out usages with time_usage <= 0
    const validUsages = processUsages.filter(usage => usage.time_usage > 0);

    if (validUsages.length === 0) {
        return res.status(200).json({ message: 'No valid process usage records to add (all time_usage are 0)' });
    }

    validUsages.forEach(({ process_title, process_path, date_recorded, start_time, time_usage }, index) => {
        processUsageModel.addProcessUsage(deviceId, process_title, process_path, date_recorded, start_time, time_usage, (err, result) => {
            if (err) {
                console.error(`Error adding process usage for record ${index}:`, err);
                errorOccurred = true;
                results.push({ error: true, message: `Error adding record ${index}` });
            } else {
                results.push({ success: true, data: result });
            }

            // Check if all records have been processed
            if (results.length === validUsages.length) {
                if (errorOccurred) {
                    return res.status(500).json({
                        message: 'Some process usage records failed to add',
                        results
                    });
                }
                res.status(201).json({
                    message: 'Process usage records added successfully',
                    results
                });
            }
        });
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