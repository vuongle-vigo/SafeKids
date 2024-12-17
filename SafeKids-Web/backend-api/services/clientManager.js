let clients = {}; // Lưu trữ các client kết nối theo ID

const addClient = (clientId, ws) => {
    console.log("add new client: ", clientId);
    clients[clientId] = ws;
};

const removeClient = (clientId) => {
    delete clients[clientId];
};

const getClient = (clientId) => {
    return clients[clientId];
};

module.exports = { addClient, removeClient, getClient };
