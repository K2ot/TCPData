#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <boost/asio.hpp>

class ClientTCP
{
	std::string address;
	std::string myPort;
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::socket socket;
	std::thread serviceThread;

	uint8_t ID;

	std::vector<unsigned char> rawData;
	ClientTCP();

	void Connect();
	void Disconnect();
	void ReceiveData();

public:

	static ClientTCP& GetInstance();

	ClientTCP(const ClientTCP&) = delete;
	ClientTCP& operator=(const ClientTCP&) = delete;
	~ClientTCP();


	bool StartClient();

	uint8_t GetID();
	uint8_t ReedID();

	
	uint8_t SendData(const std::string& data);

};