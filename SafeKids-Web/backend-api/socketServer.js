const http = require('http');
const app = require('./app');
const { initWebSocketServer } = require('./services/socketService');

const server = http.createServer(app);

// Khởi động WebSocket server
initWebSocketServer(server);

// Lắng nghe cổng
server.listen(4444, () => {
    console.log('Server is running on http://localhost:4444');
});
