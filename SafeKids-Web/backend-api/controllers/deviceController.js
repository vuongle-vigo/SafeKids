require('dotenv').config();

const configModel = require('../models/configModel');
const deviceModel = require('../models/deviceModel');
const installedAppModel = require('../models/installedAppModel');
const powerUsageModel = require('../models/powerUsageModel');
const processUsageModel = require('../models/processUsageModel');

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

const uninstallDevice = (req, res) => {
  const deviceId = req.device.device_id;

  // Kiểm tra deviceId
  if (!deviceId) {
    return res.status(400).json({ message: 'Missing or invalid deviceId' });
  }

  let completed = 0;
  let errors = [];
  const totalTasks = 5;

  // Hàm kiểm tra hoàn tất tất cả thao tác
  const checkCompletion = () => {
    completed++;
    if (completed === totalTasks) {
      if (errors.length > 0) {
        console.error('Errors during uninstall:', errors);
        return res.status(500).json({ message: 'Failed to uninstall device', errors });
      }
      return res.status(200).json({ message: 'Device and related data uninstalled successfully' });
    }
  };

  // Xóa cấu hình
  configModel.deleteConfigByDeviceId(deviceId, (err, result) => {
    if (err) {
      errors.push(`Error deleting config: ${err.message}`);
    }
    checkCompletion();
  });

    // Xóa thiết bị
  deviceModel.deleteDevice(deviceId, (err, result) => {
    if (err) {
      errors.push(`Error uninstalling device: ${err.message}`);
    }
    checkCompletion();
  });
  
  // Xóa ứng dụng đã cài
  installedAppModel.deleteAppByDeviceId(deviceId, (err, result) => {
    if (err) {
      errors.push(`Error deleting apps: ${err.message}`);
    }
    checkCompletion();
  });

  // Xóa dữ liệu sử dụng năng lượng
  powerUsageModel.deletePowerUsageByDeviceId(deviceId, (err, result) => {
    if (err) {
      errors.push(`Error deleting power usage: ${err.message}`);
    }
    checkCompletion();
  });

  // Xóa dữ liệu sử dụng quy trình
  processUsageModel.deleteProcessUsageByDeviceId(deviceId, (err, result) => {
    if (err) {
      errors.push(`Error deleting process usage: ${err.message}`);
    }
    checkCompletion();
  });
};

function getDeviceByUserId(req, res) {
    const userId = req.params.userId;
    if (!userId) {
        return res.status(400).json({ message: 'Missing or invalid userId' });
    }

    deviceModel.getDeviceByUserId(userId, (err, devices) => {
        if (err) {
            return res.status(500).json({ message: 'Error fetching devices' });
        }
        res.status(200).json({ devices });
    });
}

module.exports={
    devices,
    getDeviceById,
    deleteDevice,
    updateDeviceName,
    updateDeviceStatus,
    uninstallDevice,
    getDeviceByUserId
}
