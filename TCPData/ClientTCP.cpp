#pragma once
#include "ClientTCP.h"

ClientTCP::ClientTCP()
	: socket(ioService)
	, ID{ 0 }
{
}

ClientTCP& ClientTCP::GetInstance()
{
	static ClientTCP instance;
	return instance;
}

bool ClientTCP::StartClient()
{
	try
	{
		address = "127.0.0.1";
		myPort = "23";
		Connect();
		ReceiveData();

		return true;
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what();
		return false;
	}
}

void ClientTCP::Disconnect()
{
	ioService.stop();
	if (serviceThread.joinable())
	{
		try
		{
			serviceThread.join();
		}
		catch (const std::system_error& e)
		{
			std::cerr << e.what();
		}
	}
	if (socket.is_open())
	{
		socket.close();
	}
}

ClientTCP::~ClientTCP()
{

	Disconnect();
}

void ClientTCP::Connect()
{
	try
	{
		
		if (socket.is_open())
		{
			Disconnect();
			ioService.reset();
		}

		boost::system::error_code ec;

		socket = boost::asio::ip::tcp::socket(ioService); // utworzenie nowego gniazda
		boost::asio::ip::tcp::resolver resolver(ioService);
		boost::asio::ip::tcp::resolver::query query(address, myPort);

		
		std::cout << "Attempting to resolve address: " << address << " on port: " << myPort << std::endl;

		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, ec);

		if (ec)
		{
			std::cerr << "Error resolving address: " << ec.message() << std::endl;
			return;
		}
		boost::asio::connect(socket, endpoint_iterator);

		serviceThread = std::thread([this]
			{
				try
				{
					std::cout << "TCP Socket is open: " << socket.is_open() << std::endl;
					std::cout << "IO Service TCP is run" << std::endl;
					ioService.run();
					std::cout << "IO Service TCP is stop" << std::endl;
				}
				catch (const std::exception& e)
				{
					std::cout << "Exception in io_service :\n" << e.what() << std::endl;
				}
			});

		std::cout << "Connected";
		return;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in Connect(): " << e.what() << std::endl;
	}
}

uint8_t ClientTCP::ReedID()
{
	return ID;
}

uint8_t ClientTCP::SendData(const std::string& data)
{
	if (data.empty())
	{
		return false;
	}

	std::vector<unsigned char> dataV{ std::vector<unsigned char>(data.begin(), data.end()) };

	uint8_t id = GetID();

	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(dataV.data(), dataV.size()), error);

	if (error)
	{
		std::cerr << "Error in SendData : " << error.message() << std::endl;
		throw std::runtime_error("Error in SendData : " + error.message());
	}
	std::cout << "Send Data:\n" << data << std::endl;
	return id;
}

uint8_t ClientTCP::GetID()
{
	uint8_t id = ++ID;
	return id;
}

void ClientTCP::ReceiveData()
{
	const int DELAY{ 500 };
	rawData.resize(1024);
	auto readData = [&](const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			if (!error)
			{
				try
				{
					rawData.resize(bytes_transferred);

					std::cout << "\nReceive Data:\n";
					for (auto raw : rawData)
					{
						std::cout << raw;
					}
					std::cout << std::endl;

				}
				catch (const std::exception& e)
				{
					std::cerr << "Blad" << e.what();
				}
				ReceiveData();
			}
			else
			{
				std::cerr << "ReceiveData: Error code: " << error.value() << std::endl;
				std::cerr << "ReceiveData: Error message: " << error.message() << std::endl;

				if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset || error == boost::asio::error::would_block)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
					Disconnect();
					StartClient();
				}
			}
		};
	socket.async_read_some(boost::asio::buffer(rawData), readData);
}
