const express = require('express');
const router = express.Router();

const machineUsageController = require('../controllers/machineUsageController');
const processUsageController = require('../controllers/processUsageController');
const authMiddleware = require('../middlewares/authMiddleware');

router.post('/add-power-usage', authMiddleware.verifyKidToken, machineUsageController.addMachineUsage);
router.post('/add-process-usage', authMiddleware.verifyKidToken, processUsageController.addProcessUsage);
module.exports = router;