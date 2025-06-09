const configModel = require('../models/configModel');

function getConfig(req, res) {
    const deviceId = req.device.device_id;
    if (!deviceId) {
        return res.status(400).json({ message: 'Missing or invalid deviceId' });
    }

    configModel.getConfig(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching config' });
        }
        res.status(200).json(result);
    });
}

function getConfigDevice(req, res) {
    const { deviceId } = req.params;
    if (!deviceId) {
        return res.status(400).json({ message: 'Missing or invalid deviceId' });
    }

    configModel.getConfig(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching config for device' });
        }
        res.status(200).json(result);
    });
}

function updateTimelimitConfigDevice(req, res) {
    const { deviceId } = req.params;
    const configData = req.body;
    console.log("Updating config for device:", deviceId, "with data:", configData);
    if (!deviceId || !configData) {
        return res.status(400).json({ message: 'Missing deviceId or config data' });
    }

    configModel.updateTimelimitConfig(deviceId, configData, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error updating config for device' });
        }
        res.status(200).json({ message: 'Config updated successfully', result });
    });
}

function updateAppConfigDevice(req, res) {
    const { deviceId } = req.params;
    const configData = req.body;
    console.log("Updating app config for device:", deviceId, "with data:", configData);
    if (!deviceId || !configData) {
        return res.status(400).json({ message: 'Missing deviceId or config data' });
    }

    configModel.updateAppConfig(deviceId, configData, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error updating app config for device' });
        }
        res.status(200).json({ message: 'App config updated successfully', result });
    });
}

function updateCommandConfigDevice(req, res) {
    const { deviceId } = req.params;
    const configData = req.body["command"];
    console.log("Updating command config for device:", deviceId, "with data:", configData);
    if (!deviceId || !configData) {
        return res.status(400).json({ message: 'Missing deviceId or config data' });
    }

    configModel.updateCommandConfig(deviceId, configData, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error updating command config for device' });
        }
        res.status(200).json({ message: 'Command config updated successfully', result });
    });
}

function getCommandConfigDevice(req, res) {
    const { deviceId } = req.params;
    if (!deviceId) {
        return res.status(400).json({ message: 'Missing or invalid deviceId' });
    }

    configModel.getCommandConfigDevice(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching command config for device' });
        }
        res.status(200).json(result);
    });
}

function deleteConfigByDeviceId(req, res) {
    const { deviceId } = req.params;
    if (!deviceId) {
        return res.status(400).json({ message: 'Missing or invalid deviceId' });
    }

    configModel.deleteConfigByDeviceId(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error deleting config for device' });
        }
        res.status(200).json({ message: 'Config deleted successfully', result });
    });
}

module.exports = {
    getConfig,
    getConfigDevice,
    updateTimelimitConfigDevice,
    updateAppConfigDevice,
    updateCommandConfigDevice,
    getCommandConfigDevice,
    deleteConfigByDeviceId
};