//Client1
#pragma comment (lib,"ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <time.h>
#include <Windows.h>
#include <exception>
#include <limits>
#include <cstddef>
#include "Protocol.h"

std::string timeGenerator() {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string str(buffer);
	return str;
}

int main() {
	Protocol wysylany, odbierany;
	//inicjalizacja WinSock-1
	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 2);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		MessageBox(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	SOCKET out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (out == INVALID_SOCKET) {
		std::cout << "Error creating socket: " << WSAGetLastError();
		WSACleanup();
		system("pause");
		return 0;
	}

	SOCKADDR_IN server;
	int server_len = sizeof(server);
	InetPton(AF_INET, "51.68.136.85", &server.sin_addr.s_addr);//ip 
	server.sin_port = htons(37000);//port 54000
	server.sin_family = AF_INET;

	char recvBuffer[150];
	char sendBuffer[150];

	bool noEntry = false;

	int Tries;
	int iSendResult = SOCKET_ERROR, iRecvResult;


	wysylany.setOp("HELLO");
	wysylany.setOdp("OK");
	wysylany.setTime(timeGenerator());

	//wysylanie powitalnego pakietu
	
	iSendResult = sendto(out, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&server, server_len);
	

	iRecvResult = recvfrom(out, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&server, &server_len);
	
	odbierany = getProtocol(recvBuffer);

	std::cout << "Uzyskane ID: " << odbierany.getId() << std::endl;

	int number = -1;
		
	do {
		try {
			while (std::cout << "Podaj dowolna liczbe L parzysta z przedzialu od 1 do 30: " && !(std::cin >> number)) {
				std::cin.clear(); //clear bad input flag
				std::cin.ignore(100, '\n'); //discard input
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			continue;
		}
	} while (number & 2 != 0 || number > 31 || number < 1);

	wysylany.setId(odbierany.getId());
	wysylany.setOp("NUMBER_L");
	wysylany.setOdp("OK");
	wysylany.setNum(std::to_string(number));
	wysylany.setTime(timeGenerator());

	//wysylanie liczby L
	iSendResult = sendto(out, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&server, server_len);
	

	//odieranie liczby prob
	iRecvResult = recvfrom(out, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&server, &server_len);
	
	
	odbierany = getProtocol(recvBuffer);

	Tries = stoi(odbierany.getNum());
	std::cout << "Liczba prob: " << Tries << std::endl << std::endl;

	//zgdywanie liczby
	for (int i = 0; i < Tries; i++) {
		number = -1;
		do {
			try {
				while (std::cout << "Podaj liczbe z zakresu od 1 do 20: " && !(std::cin >> number)) {
					std::cin.clear(); 
					std::cin.ignore(100, '\n'); 
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				continue;
			}
		} while (number > 21 || number < 0);
		//wysylanie liczby
		wysylany.setNum(std::to_string(number));
		wysylany.setOp("NUMBER");
		wysylany.setOdp("OK");
		wysylany.setTime(timeGenerator());
		iSendResult = sendto(out, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&server, server_len);
		

		iRecvResult = recvfrom(out, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&server, &server_len);
		
		
		odbierany = getProtocol(recvBuffer);
		

		if (odbierany.getOp() == "RESULT"&&odbierany.getOdp() == "LOST") {
			std::cout << "Przegrales!! Powodzenia nastepnym razem." << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			break;
		}
		else if (odbierany.getOp() == "RESULT"&&odbierany.getOdp() == "WIN") {
			std::cout << "Gratulacje!! Wygrales!" << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			break;
		}
		else if (odbierany.getOp() == "RESULT"&&odbierany.getOdp() == "WRONG") {
			if (i == Tries - 1) {
				std::cout << "Remis, nikt nie wygral. Obaj gracze wykorzystali wszsytkie proby" << std::endl;
				std::cout << "-------------------------------------------------------" << std::endl;
			}
			else {
				std::cout << "Pudlo! Sprobuj jeszcze raz" << std::endl;
				std::cout << "-------------------------------------------------------" << std::endl;

			}
		}

	}
	std::cout << "Zamykanie polacznenia..." << std::endl;
	closesocket(out);
	WSACleanup();
	system("pause");
	return 0;
}