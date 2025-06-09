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

function addInstalledApps(req, res) {
    const deviceId = req.device.device_id;
    // Check if request body is an array
    if (!Array.isArray(req.body)) {
        console.error("Invalid request body: Expected an array");
        return res.status(400).json({ error: "Request body must be an array of applications" });
    }

    // Check if the array is empty
    if (req.body.length === 0) {
        console.warn("Empty application array");
        return res.status(400).json({ error: "No applications provided" });
    }

    const results = [];
    const errors = [];

    // Loop app in array
    req.body.forEach((app, index) => {
        const { app_name, exe_path, install_location, publisher, version, uninstall_string, quiet_uninstall_string, status } = app;

        // Check if app_name is valid
        if (!app_name || typeof app_name !== 'string' || app_name.trim() === '') {
            console.error(`Invalid app_name at index ${index}:`, app_name);
            errors.push({ index, error: "Missing or invalid app_name" });
            return;
        }

        // Call model to add installed app
        installedAppModel.addInstalledApps(
            deviceId,
            app_name,
            exe_path || '',
            install_location || '',
            publisher || '',
            version || '',
            uninstall_string || '',
            quiet_uninstall_string || '',
            status || 'installed',
            (err, result) => {
                if (err) {
                    console.error(`Error adding app "${app_name}" at index ${index}:`, err);
                    errors.push({ index, app_name, error: err.message });
                } else {
                    results.push({ index, app_name, result });
                }

                // Check if all applications have been processed
                if (results.length + errors.length === req.body.length) {
                    if (errors.length > 0) {
                        console.warn("Some applications failed to add:", errors);
                        return res.status(207).json({
                            message: "Some applications were added successfully, but errors occurred",
                            results,
                            errors
                        });
                    }
                    res.status(201).json({
                        message: "All applications added successfully",
                        results
                    });
                }
            }
        );
    });
}

function deleteAppByDeviceId(req, res) {
    const { deviceId } = req.params;
    installedAppModel.deleteAppByDeviceId(deviceId, (err, result) => {
        if (err) {
            console.error('Error deleting apps for device:', err);
            return res.status(500).json({ error: 'Failed to delete apps' });
        }
        res.status(200).json({ message: 'Apps deleted successfully', result });
    });
}

module.exports = {
    getInstalledAppsByDevice,
    addInstalledApps,
    deleteAppByDeviceId
};
