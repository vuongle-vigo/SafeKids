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

function getDeviceById(req, res) {
    const deviceId = req.params.deviceId;
    deviceModel.getDeviceById(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching device' });
        }
        res.status(200).json(result);
    });
}

function deleteDevice(req, res) {
    const deviceId = req.params.deviceId;
    console.log('Device ID to delete:', deviceId);
    deviceModel.deleteDevice(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error deleting device' });
        }
        res.status(200).json({ message: 'Device deleted successfully' });
    });
}

function updateDeviceName(req, res) {
    const deviceId = req.params.deviceId;
    const { device_name } = req.body;

    deviceModel.updateDeviceName(deviceId, device_name, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error updating device name' });
        }
        res.status(200).json({ message: 'Device name updated successfully' });
    });
}

function updateDeviceStatus(req, res) {
    const deviceId = req.device.device_id;
    if (!deviceId) {
        return res.status(400).json({ message: 'Missing or invalid deviceId' });
    }

    deviceModel.updateDeviceStatus(deviceId, (err, result) => {
        if (err) {
            return res.status(500).json({ message: 'Error updating device status' });
        }
        res.status(200).json({ message: 'Device status updated successfully' });
    });
}

module.exports={
    devices,
    getDeviceById,
    deleteDevice,
    updateDeviceName,
    updateDeviceStatus
}
