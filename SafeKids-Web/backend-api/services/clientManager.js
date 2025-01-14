const deviceModel = require('../models/deviceModel');

let clients = {}; // Lưu trữ các client kết nối theo ID

const addClient = (clientId, ws) => {
    console.log("add new client: ", clientId);
    clients[clientId] = ws;
};

const removeClient = (ws) => {
    const clientId = Object.keys(clients).find((key) => clients[key] === ws);
    if (clientId) {
        delete clients[clientId];
        deviceModel.uploadLastActivity(clientId);
        console.log(`Client ${clientId} removed.`);
    } else {
        console.log('WebSocket connection not found in client list.');
    }
};

const getClient = (clientId) => {
    return clients[clientId];
};

const getClientIds = () => {
    return Object.keys(clients);
};

module.exports = { addClient, removeClient, getClient, getClientIds };
