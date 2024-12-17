const express = require('express');
const router = express.Router();

const commandController = require('../controllers/commandController');

router.post('/sendCommand', commandController.sendCommand);
router.get('/getClientsOnline', commandController.getClientsOnline);

module.exports = router;
