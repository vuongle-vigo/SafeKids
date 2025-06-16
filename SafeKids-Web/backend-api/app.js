require('dotenv').config();
const express = require('express');
const path = require('path');
const cookieParser = require('cookie-parser');
const logger = require('morgan');
const cors = require('cors');

const userRoutes = require('./routes/userRoutes');
const authRoutes = require('./routes/authRoutes');
const deviceRoutes = require('./routes/deviceRoutes');
const commandRoutes = require('./routes/commandRoutes');
const kidRoutes = require('./routes/kidRoutes');
const adminRoutes = require('./routes/adminRoutes');

const app = express();

// Biến môi trường
const PORT = process.env.PORT || 8889; // Render gán PORT động
const FRONTEND_URL = process.env.FRONTEND_URL || '*'; // URL frontend từ env hoặc '*' cho dev

// Middleware
app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

// Cấu hình CORS
app.use(cors({
  origin: FRONTEND_URL,
  methods: ['GET', 'POST', 'OPTIONS', 'PUT', 'DELETE'],
  allowedHeaders: ['Authorization', 'Content-Type'],
  credentials: true
}));

// Route API
app.use('/api/user', userRoutes);
app.use('/api/auth', authRoutes);
app.use('/api/devices', deviceRoutes);
app.use('/api/commands', commandRoutes);
app.use('/api/kid', kidRoutes);
app.use('/api/admin', adminRoutes);

// Phục vụ tệp tĩnh frontend
app.use(express.static(path.join(__dirname, '../frontend-safekids')));

// Route gốc
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '../frontend-safekids', 'index.html'));
});

// Middleware xử lý lỗi
app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(500).json({ message: 'Đã xảy ra lỗi server' });
});

// Khởi động server
app.listen(PORT, () => {
  console.log(`Server đang chạy trên cổng ${PORT}`);
});

module.exports = app;