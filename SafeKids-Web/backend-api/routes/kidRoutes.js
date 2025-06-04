const express = require('express');
const router = express.Router();

const machineUsageController = require('../controllers/powerUsageController');
const processUsageController = require('../controllers/processUsageController');
const installedAppController = require('../controllers/installedAppController');
const configController = require('../controllers/configController');
const authMiddleware = require('../middlewares/authMiddleware');
const deviceController = require('../controllers/deviceController');

router.post('/add-power-usage', authMiddleware.verifyKidToken, machineUsageController.addPowerUsage);
router.post('/add-process-usage', authMiddleware.verifyKidToken, processUsageController.addProcessUsage);
router.post('/add-installed-apps', authMiddleware.verifyKidToken, installedAppController.addInstalledApps);
router.get('/get-config', authMiddleware.verifyKidToken, configController.getConfig);
router.post('/update-status', authMiddleware.verifyKidToken, deviceController.updateDeviceStatus);

module.exports = router;