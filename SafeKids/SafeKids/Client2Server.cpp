#include "Client2Server.h"
#include "common.cpp"

Client2Server::Client2Server() {
	m_wsClient.init_asio();
	m_wsClient.set_open_handler([this](websocketpp::connection_hdl hdl) {
		this->OnOpen(hdl);
		});
	m_wsClient.set_message_handler([this](websocketpp::connection_hdl hdl, client::message_ptr msg) {
		this->OnMessage(hdl, msg);
		});
	m_wsClient.set_close_handler([this](websocketpp::connection_hdl hdl) {
		this->OnClose(hdl);
		});
}

Client2Server::~Client2Server() {
	Close();
}

Client2Server& Client2Server::GetInstance() {
	static Client2Server instance;
	return instance;
}

void Client2Server::Connect() {
	Config& conf = Config::GetInstance();
	std::string szUri = "ws://" + conf.GetHost() + ":" + std::to_string(conf.GetPort());
	websocketpp::lib::error_code error_code;
	/*client::connection_ptr conn = m_wsClient.get_connection(szUri, error_code);
	if (error_code) {
		DEBUG_LOG("Cannot connect to server: %s", szUri.c_str());
		return;
	}


	m_wsClient.connect(conn);
	DEBUG_LOG("Connect0");
	m_wsThread = std::thread([this]() {
		m_wsClient.run();
		});*/

	try {
		// T?o k?t n?i
		client::connection_ptr conn = m_wsClient.get_connection(szUri, error_code);

		if (error_code) {
			throw std::runtime_error("Cannot get connection: " + error_code.message());
		}

		// Th?c hi?n k?t n?i
		m_wsClient.connect(conn);
		DEBUG_LOG("Connect0");

		// Ch?y WebSocket trong m?t thread riêng
		m_wsThread = std::thread([this]() {
			try {
				m_wsClient.run();
			}
			catch (const std::exception& e) {
				DEBUG_LOG("Error in WebSocket run: %s", e.what());
				Close();  // ??m b?o k?t n?i ???c ?óng khi có l?i
			}
			});

	}
	catch (const std::exception& e) {
		DEBUG_LOG("Exception during connection: %s", e.what());
		Close(); // N?u có l?i, ??m b?o k?t n?i ???c ?óng
	}

	//if (!m_bIsConnected) {
	//	Close();
	//}

	DEBUG_LOG("Connect");
}

void Client2Server::SendMessage2Server(const std::string& message) {
	websocketpp::lib::error_code error_code;
	m_wsClient.send(m_wsHdl, message, websocketpp::frame::opcode::text, error_code);
	if (error_code) {
		DEBUG_LOG("Error send message to server: %s", error_code.message().c_str());
	}
}

void Client2Server::Close() {
	websocketpp::lib::error_code error_code;
	m_wsClient.close(m_wsHdl, websocketpp::close::status::going_away, "", error_code);
	if (error_code) {
		DEBUG_LOG("Error close connect: %s", error_code.message().c_str());
	}

	if (m_wsThread.joinable()) {
		m_wsThread.join();
	}

	DEBUG_LOG("Close success");
}

void Client2Server::OnOpen(websocketpp::connection_hdl hdl) {
	DEBUG_LOG("Connect success");
	m_wsHdl = hdl;
	m_bIsConnected = true;
}

void Client2Server::OnMessage(websocketpp::connection_hdl hdl, client::message_ptr msg) {
	std::string message = msg->get_payload();
	DEBUG_LOG("Received message: %s", message.c_str());
}

void Client2Server::OnClose(websocketpp::connection_hdl hdl) {
	DEBUG_LOG("Connect closed");
	m_bIsConnected = false;
	/*websocketpp::lib::error_code error_code;
	m_wsClient.close(m_wsHdl, websocketpp::close::status::going_away, "", error_code);
	if (error_code) {
		DEBUG_LOG("Error closing connection: %s", error_code.message().c_str());
	}

	if (m_wsThread.joinable()) {
		m_wsThread.join();
	}*/
}

void Client2Server::SetFlagConnected(bool bIsConnected) {
	m_bIsConnected = bIsConnected;
}

bool Client2Server::GetFlagConnected() {
	return m_bIsConnected;
}