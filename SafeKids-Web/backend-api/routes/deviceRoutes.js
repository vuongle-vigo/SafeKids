const express = require('express');
const router = express.Router();

const deviceController = require('../controllers/deviceController');
const authMiddleware  = require('../middlewares/authMiddleware');

router.get('/', authMiddleware.verifyToken, deviceController.devices);

module.exports = router;