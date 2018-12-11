//Server
#pragma comment (lib,"ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <time.h>
#include <Windows.h>
#include "Protocol.h"


int randomNumber(const int &min, const int&max) {
	return rand() % (max - min) + 1;
}

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
	srand(time(NULL));
	Protocol odbierany, wysylany;
	//inicjalizacja WinSock-a
	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 2);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		MessageBox(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//tworzenie gniazda lokalnego
	SOCKET in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (in == INVALID_SOCKET) {
		std::cout << "Error creating socket: " << WSAGetLastError();
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN lockalAddr;
	int lockal_addrlen = sizeof(lockalAddr);
	InetPton(AF_INET, "127.0.0.1", &lockalAddr.sin_addr.s_addr);
	lockalAddr.sin_port = htons(54000);
	lockalAddr.sin_family = AF_INET;

	if (bind(in, (SOCKADDR*)&lockalAddr, lockal_addrlen) == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(in);
		WSACleanup();
		system("pause");
		return 1;
	}

	//zmienne
	int iRecvResult, iSendResult;
	char recvBuff[150] = "";
	char sendBuff[150] = "";
	bool ifEnd = false;

	std::string ID_1;
	std::string ID_2;

	int randID_1, randID_2, secretNumber;
	int tries = 0;

	int counter = 0;

	std::cout << "Oczekiwane na graczy..." << std::endl;
	

	//client
	randID_1 = randomNumber(1, 10);

	do {
		randID_2 = randomNumber(1, 10);
	} while (randID_1 == randID_2);

	ID_1 = std::to_string(randID_1);
	ID_2 = std::to_string(randID_2);

	//klient 1
	SOCKADDR_IN client_1;
	int client1_len = sizeof(client_1);
	//ZeroMemory(&client_1, client1_len);

	//klient 2
	SOCKADDR_IN client_2;
	int client2_len = sizeof(client_2);
	//ZeroMemory(&client_2, client2_len);
	ZeroMemory(&client_1, client1_len); // Clear the client structure
	ZeroMemory(&client_2, client2_len);

	secretNumber = randomNumber(1, 20);
	std::cout << "Wylosowana liczba tajna: " << secretNumber << std::endl;

	do {
		//oczekiwanie na klientów
		if (counter == 0) {
			iRecvResult = recvfrom(in, recvBuff, sizeof(recvBuff), 0, (SOCKADDR*)&client_1, &client1_len);
			std::cout << "Polaczono z klientem 1. Wygenerowane ID: " << ID_1 << std::endl;
							
			iRecvResult = recvfrom(in, recvBuff, sizeof(recvBuff), 0, (SOCKADDR*)&client_2, &client2_len);
			std::cout << "Polaczono z klientem 2. Wygenerowany ID: " << ID_2 << std::endl;
			
			wysylany.setOp("HELLO");
			wysylany.setOdp("OK");
			wysylany.setId(ID_1);
			wysylany.setTime(timeGenerator());
			iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_1, client1_len);

			wysylany.setId(ID_2);
			wysylany.setTime(timeGenerator());
			iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_2, client2_len);
			
			

			
			//odbieranie pakietu z liczbami L1 i L2
			iRecvResult = recvfrom(in, recvBuff, sizeof(recvBuff), 0, (SOCKADDR*)&client_1, &client1_len);
			

			odbierany = getProtocol(recvBuff);
			tries = tries + stoi(odbierany.getNum());

			iRecvResult = recvfrom(in, recvBuff, sizeof(recvBuff), 0, (SOCKADDR*)&client_2, &client2_len);
			

			odbierany = getProtocol(recvBuff);
			tries = tries + stoi(odbierany.getNum());

			tries = tries / 2;

			std::cout << "Liczba prob zostala ustalona: " << tries << std::endl;

			wysylany.setOp(odbierany.getOp());
			wysylany.setNum(std::to_string(tries));
			wysylany.setTime(timeGenerator());

			iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_1, client1_len);

			wysylany.setId(ID_1);
			wysylany.setTime(timeGenerator());
			iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_2, client2_len);

			counter++;

		}
		else {
			iRecvResult = recvfrom(in, recvBuff, sizeof(recvBuff), 0, (SOCKADDR*)&client_1, &client1_len);
			
			odbierany = getProtocol(recvBuff);
			int id1 = stoi(odbierany.getId());
			int n1 = stoi(odbierany.getNum());
			std::cout << "Klient o ID: " << odbierany.getId() << ": " << odbierany.getNum() << std::endl;


			iRecvResult = recvfrom(in, recvBuff, sizeof(recvBuff), 0, (SOCKADDR*)&client_2, &client2_len);
			

			odbierany = getProtocol(recvBuff);
			int id2 = stoi(odbierany.getId());
			int n2 = stoi(odbierany.getNum());
			std::cout << "Klient o ID: " << odbierany.getId() << ": " << odbierany.getNum() << std::endl;

			tries--;

			//sprawdzanie czy wyslana liczba == tajnej liczbie
			if (n1 == secretNumber && n2 != secretNumber) {
				wysylany.setOp("RESULT");
				wysylany.setOdp("WIN");
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_1, client1_len);
				

				wysylany.setOdp("LOST");
				wysylany.setId(std::to_string(id2));
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_2, client2_len);
				std::cout << "Koniec gry. Wygral klient o ID: " << id1 << std::endl;
				break;
			}
			else if (n1 != secretNumber && n2 == secretNumber) {
				wysylany.setOp("RESULT");
				wysylany.setOdp("WIN");
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_2, client2_len);
				wysylany.setOdp("LOST");
				wysylany.setId(std::to_string(id2));
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_1, client1_len);
				std::cout << "Koniec gry. Wygral klient o ID: " << id2 << std::endl;
				break;
			}
			else if (n1 != secretNumber && n2 != secretNumber && tries > 0) {

				wysylany.setOp("RESULT");
				wysylany.setOdp("WRONG");
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_1, client1_len);
				wysylany.setId(std::to_string(id2));
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_2, client2_len);
			}
			else if (tries == 0) {

				wysylany.setOp("RESULT");
				wysylany.setOdp("WRONG");
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_1, client1_len);
				wysylany.setId(std::to_string(id1));
				wysylany.setTime(timeGenerator());
				iSendResult = sendto(in, wysylany.getString().c_str(), wysylany.getString().size(), 0, (SOCKADDR*)&client_2, client2_len);
				break;

			}
		}
	} while (iSendResult>0);

	std::cout << "Zamykanie polaczenia...." << std::endl;
	closesocket(in);
	WSACleanup();
	system("pause");
	return 0;
}