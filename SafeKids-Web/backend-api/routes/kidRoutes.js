const express = require('express');
const router = express.Router();

const machineUsageController = require('../controllers/powerUsageController');
const processUsageController = require('../controllers/processUsageController');
const authMiddleware = require('../middlewares/authMiddleware');

router.post('/add-power-usage', authMiddleware.verifyKidToken, machineUsageController.addPowerUsage);
router.post('/add-process-usage', authMiddleware.verifyKidToken, processUsageController.addProcessUsage);

module.exports = router;