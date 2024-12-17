const clientManager = require('./clientManager');

const sendCommandToClient = (clientId, command) => {
    return new Promise((resolve, reject) => {
        const client = clientManager.getClient(clientId);
        if (client) {
            client.send(JSON.stringify({ command }));
            resolve(`Command '${command}' sent to client ${clientId}`);
        } else {
            reject(new Error(`Client ${clientId} not connected`));
        }
    });
};

module.exports = { sendCommandToClient };
