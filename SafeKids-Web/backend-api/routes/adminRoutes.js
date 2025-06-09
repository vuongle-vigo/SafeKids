const express = require('express')
const router = express.Router();
const authMiddleware  = require('../middlewares/authMiddleware');
const userController = require('../controllers/userController');

router.get('/get-all-users', authMiddleware.verifyToken, userController.getAllUsers);
router.post('/create-user', authMiddleware.verifyToken, userController.createUser);
router.delete('/delete-user/:id', authMiddleware.verifyToken, userController.deleteUser);
router.put('/change-password/:id', authMiddleware.verifyToken, userController.changePassword);

module.exports = router;