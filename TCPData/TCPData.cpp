#include <iostream>
#include "ClientTCP.h"


int main()
{
	setlocale(LC_CTYPE, "Polish");
	bool stopFlag{ false };

	ClientTCP::GetInstance().StartClient();

	while (!stopFlag)
	{
		std::string message{ "" };
		std::getline(std::cin, message);
		if (message == "stop" || message == "koniec" || message == "end")
		{
			stopFlag = true;
		}
		else
		{
			ClientTCP::GetInstance().SendData(message);
		}
		message.clear();
	}

}
