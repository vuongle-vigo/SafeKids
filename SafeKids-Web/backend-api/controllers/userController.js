const userModel = require('../models/userModel');
const bcrypt = require('bcrypt');

// Lấy tất cả người dùng
function getAllUsers(req, res) {
  userModel.getAllUsers((err, users) => {
    if (err) {
      return res.status(500).json({ message: 'Error fetching users' });
    }
    res.status(200).json({ users });
  });
}

function createUser(req, res) {
  const { email, password } = req.body;

  userModel.findUserByEmail(email, (err, existingUser) => {
    if (err) {
      return res.status(500).json({ message: 'Error checking email' });
    }
    if (existingUser) {
      return res.status(400).json({ message: 'Email already exists' });
    }
  });

  if (!email || !password) {
    return res.status(400).json({ message: 'Email and password are required' });
  }

  // Tạo người dùng mới
  const hashedPassword = bcrypt.hashSync(password, 10); // Mã hóa mật khẩu

  userModel.createUser(email, hashedPassword, (err, user) => {
    if (err) {
      return res.status(500).json({ message: 'Error creating user' });
    }
    res.status(201).json({ message: 'User created successfully', user });
  });
}

// Lấy người dùng theo ID
function getUserById(req, res) {
  const userId = parseInt(req.params.id);
  userModel.getUserById(userId, (err, user) => {
    if (err) {
      return res.status(500).json({ message: 'Error fetching user' });
    }
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }
    res.status(200).json(user);
  });
}

function deleteUser(req, res) {
  const userId = parseInt(req.params.id);
  userModel.deleteUser(userId, (err) => {
    if (err) {
      return res.status(500).json({ message: 'Error deleting user' });
    }
    res.status(200).json({ message: 'User deleted successfully' });
  });
}

function changePassword(req, res) {
  const { newPassword } = req.body;
  const userId = parseInt(req.params.id);
  console.log('Changing password for userId:', userId);
  //hash
  const hashedPassword = bcrypt.hashSync(newPassword, 10); // Mã hóa mật khẩu

  userModel.changePassword(userId, hashedPassword, (err) => {
    if (err) {
      return res.status(500).json({ message: 'Error changing password' });
    }
    res.status(200).json({ message: 'Password changed successfully' });
  });
}

module.exports = {
  getAllUsers,
  createUser,
  getUserById,
  deleteUser,
  changePassword,
}