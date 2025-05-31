const installedAppModel = require('../models/installedAppModel');

function getInstalledAppsByDevice(req, res) {
    const { deviceId } = req.params;
    installedAppModel.getInstalledAppsByDeviceId(deviceId, (err, installedApps) => {
        if (err) {
            console.error('Error fetching installed apps:', err);
            return res.status(500).json({ error: 'Failed to fetch installed apps' });
        }

        if (!installedApps || installedApps.length === 0) {
            return res.status(404).json({ message: 'No installed apps found for this device' });
        }
        res.status(200).json(installedApps);
    });
}

module.exports = {
    getInstalledAppsByDevice
};
