require('dotenv').config();
const http = require('http');

var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
const cors = require('cors');

const userRoutes = require('./routes/userRoutes');
const authRoutes = require('./routes/authRoutes')
const deviceRoutes = require('./routes/deviceRoutes');
const commandRoutes = require('./routes/commandRoutes');
const { initWebSocketServer } = require('./services/socketService'); 
// const timeRoutes = require('./routes/timeRoutes'); // Removed

// var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');

var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use(cors({
    origin: 'http://localhost:5173',  
    methods: ['GET', 'POST', 'OPTIONS'],  
    allowedHeaders: ['Authorization', 'Content-Type'],  
  }));

app.use('/users', usersRouter);
app.use('/api/users', userRoutes);
app.use('/api/auth', authRoutes);
app.use('/api/devices', deviceRoutes);
app.use('/api/commands', commandRoutes);

app.use(express.static(path.join(__dirname, '../frontend-safekids')));

const server = http.createServer(app);
initWebSocketServer(server);

const PORT = process.env.PORT || 3000;
const HOST = process.env.HOST || 'localhost';

server.listen(PORT, HOST, () => {
  console.log(`Server is running at http://${HOST}:${PORT}`);
  console.log(`WebSocket server is running at ws://${HOST}:${PORT}`);
});

module.exports = app;
