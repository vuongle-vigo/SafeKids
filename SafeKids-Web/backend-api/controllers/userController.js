// Giả sử đây là dữ liệu giả lập (mock data)
let users = [
    { id: 1, name: 'John Doe', email: 'john@example.com' },
    { id: 2, name: 'Jane Smith', email: 'jane@example.com' }
  ];
  
  // Lấy tất cả người dùng
  exports.getAllUsers = (req, res) => {
    res.status(200).json(users);
  };
  
  // Tạo người dùng mới
  exports.createUser = (req, res) => {
    const { name, email } = req.body;
  
    if (!name || !email) {
      return res.status(400).json({ message: 'Name and email are required' });
    }
  
    const newUser = {
      id: users.length + 1,
      name,
      email
    };
  
    users.push(newUser);
    res.status(201).json(newUser);
  };
  
  // Lấy người dùng theo ID
  exports.getUserById = (req, res) => {
    const userId = parseInt(req.params.id);
    const user = users.find(u => u.id === userId);
  
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }
  
    res.status(200).json(user);
  };
  