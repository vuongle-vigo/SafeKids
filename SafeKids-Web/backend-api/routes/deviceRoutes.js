const express = require('express');
const router = express.Router();

const deviceController = require('../controllers/deviceController');
const authMiddleware  = require('../middlewares/authMiddleware');
const timeController = require('../controllers/timeController');

router.get('/', authMiddleware.verifyToken, deviceController.devices);
router.get('/:deviceId', authMiddleware.verifyToken, deviceController.getDeviceById);
router.get('/:deviceId/screen-time', authMiddleware.verifyToken, timeController.getScreenTime);

module.exports = router;