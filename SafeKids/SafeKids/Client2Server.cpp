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
	//Close();
}

void Client2Server::Connect() {
	Config& conf = Config::GetInstance();
	std::string szUri = "ws://" + conf.GetHost() + ":" + conf.GetPort();
	websocketpp::lib::error_code error_code;
	client::connection_ptr conn = m_wsClient.get_connection(szUri, error_code);
	if (error_code) {
		DEBUG_LOG("Connot connect to server: %s", szUri.c_str());
		return;
	}

	m_wsClient.connect(conn);
	m_wsThread = std::thread([this]() {
		m_wsClient.run();
		});

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
}

void Client2Server::OnOpen(websocketpp::connection_hdl hdl) {
	DEBUG_LOG("Connect success");
	m_wsHdl = hdl;
}

void Client2Server::OnMessage(websocketpp::connection_hdl hdl, client::message_ptr msg) {
	std::string message = msg->get_payload();
	DEBUG_LOG("Received message: %s", message.c_str());
}


void Client2Server::OnClose(websocketpp::connection_hdl hdl) {
	DEBUG_LOG("Connect closed");
}