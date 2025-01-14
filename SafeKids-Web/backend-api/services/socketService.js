const WebSocket = require('ws');
const clientManager = require('./clientManager');

const initWebSocketServer = (server) => {
    const wss = new WebSocket.Server({ server });

    wss.on('connection', (ws) => {
        console.log("Have connection");
        // const clientId = clientManager.addClient(ws);
        ws.on('message', (message) => {
            console.log(`Message client:`, message.toString());
            try {
                const data = JSON.parse(message);
                switch (data.type) {
                    case 'REGISTER':
                        if (data.clientId) {
                            clientManager.addClient(data.clientId, ws);
                        }
                        break;
                
                    default:
                        break;
                }
            } catch(err) {
                console.error('Error parsing message:', err.message);
            }
            
        });

        ws.on('close', () => {
            clientManager.removeClient(ws);
            console.log(`Client disconnected.`);
        });
    });

    return wss;
};

module.exports = { initWebSocketServer };
