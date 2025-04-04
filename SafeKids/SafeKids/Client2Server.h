//#pragma once
//#include "websocketpp/client.hpp"
//#include "websocketpp/config/asio_no_tls_client.hpp"
//#include "Config.h"
//
//typedef websocketpp::client<websocketpp::config::asio_client> client;
//
//class Client2Server {
//public:
//	Client2Server();
//	~Client2Server();
//
//	static Client2Server& GetInstance();
//	void Connect();
//	void SendMessage2Server(const std::string& message);
//	void Close();
//	void SetFlagConnected(bool bIsConnected);
//	bool GetFlagConnected();
//private:
//	client m_wsClient;
//	std::thread m_wsThread;
//	websocketpp::connection_hdl m_wsHdl;
//	bool m_bIsConnected;
//	void OnOpen(websocketpp::connection_hdl hdl);
//	void OnMessage(websocketpp::connection_hdl hdl, client::message_ptr msg);
//	void OnClose(websocketpp::connection_hdl);
//};