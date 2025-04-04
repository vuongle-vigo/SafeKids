const express = require('express');
const router = express.Router();

const deviceController = require('../controllers/deviceController');
const authMiddleware  = require('../middlewares/authMiddleware');
const timeController = require('../controllers/timeController');
const machineUsageController = require('../controllers/machineUsageController');

router.get('/', authMiddleware.verifyToken, deviceController.devices);
router.get('/:deviceId', authMiddleware.verifyToken, deviceController.getDeviceById);
router.get('/:deviceId/screen-time', authMiddleware.verifyToken, timeController.getScreenTime);
router.get('/:deviceId/machine-usage', authMiddleware.verifyToken, machineUsageController.getMachineUsageByTime);

module.exports = router;