const express = require('express');
const router = express.Router();
const { sendCommandToClient } = require('../services/commandService');

// Endpoint gửi lệnh đến client thông qua WebSocket
async function sendCommand(req, res) {
    const { clientId, command } = req.body;
    try {
        const result = await sendCommandToClient(clientId, command);
        res.status(200).json({ status: 'success', message: result });
    } catch (error) {
        res.status(400).json({ status: 'error', message: error.message });
    }
}

function getClientsOnline(req, res) {
    const clientManager = require('../services/clientManager');
    const connectedClients = clientManager.getClientIds();
    if (connectedClients) {
        res.status(200).json({
            status: 'success',
            clients: connectedClients
        });
    } else {
        res.status(200).json({
            status: 'success',
            clients: ""
        });
    } 
}

module.exports = {
    sendCommand, 
    getClientsOnline
};
