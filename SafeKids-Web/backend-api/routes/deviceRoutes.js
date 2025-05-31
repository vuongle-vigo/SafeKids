const express = require('express');
const router = express.Router();

const deviceController = require('../controllers/deviceController');
const authMiddleware  = require('../middlewares/authMiddleware');
const powerUsageController = require('../controllers/powerUsageController');
const processUsageController = require('../controllers/processUsageController');
const installedAppController = require('../controllers/installedAppController');

router.get('/', authMiddleware.verifyToken, deviceController.devices);
router.get('/:deviceId', authMiddleware.verifyToken, deviceController.getDeviceById);
router.get('/:deviceId/power-usage', authMiddleware.verifyToken, powerUsageController.getPowerUsage);
router.post('/:deviceId/power-usage/time', authMiddleware.verifyToken, powerUsageController.getPowerUsageByTime);
router.post('/:deviceId/process-usage', authMiddleware.verifyToken, processUsageController.getProcessUsageByDeviceAndDate);
router.get('/:deviceId/installed-apps', authMiddleware.verifyToken, installedAppController.getInstalledAppsByDevice);

module.exports = router;