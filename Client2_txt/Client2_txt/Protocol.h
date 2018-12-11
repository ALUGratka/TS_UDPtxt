#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <WinSock2.h>
#include <string>
#include <regex>
#include <fstream>
#include <iomanip>
#include <ctime>

class Protocol {
private:
	std::string Operacja = "";
	std::string Odpowiedz = "";
	std::string Numer = "";
	std::string Identyfikator = "";
	std::string Time = "";

public:
	std::string getOp();
	std::string getOdp();
	std::string getNum();
	std::string getId();
	std::string getTime();

	void setOp(std::string s);
	void setOdp(std::string s);
	void setNum(std::string s);
	void setId(std::string s);
	void setTime(std::string s);

	std::string getString() {
		return "Operacja>" + Operacja + "<Odpowiedz>" + Odpowiedz + "<Numer>" + Numer + "<Identyfikator>" + Identyfikator + "<Czas>" + Time + "<";
	}
	friend std::ostream& operator<<(std::ostream&out, Protocol &p);
};

Protocol getProtocol(std::string message);

