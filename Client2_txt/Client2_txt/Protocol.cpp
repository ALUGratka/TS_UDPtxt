#include "Protocol.h"

std::string Protocol::getOp()
{
	return this->Operacja;
}

std::string Protocol::getOdp()
{
	return this->Odpowiedz;
}

std::string Protocol::getNum()
{
	return this->Numer;
}

std::string Protocol::getId()
{
	return this->Identyfikator;
}

std::string Protocol::getTime()
{
	return this->Time;
}

void Protocol::setOp(std::string s)
{
	this->Operacja = s;
}

void Protocol::setOdp(std::string s)
{
	this->Odpowiedz = s;
}

void Protocol::setNum(std::string s)
{
	this->Numer = s;
}

void Protocol::setId(std::string s)
{
	this->Identyfikator = s;
}

void Protocol::setTime(std::string s)
{
	this->Time = s;
}



std::ostream & operator<<(std::ostream & out, Protocol & p)
{
	auto time = std::time(nullptr);
	out << std::put_time(std::localtime(&time), "%T ") << " | Operacja: " << p.Operacja << " | Odpowiedz: " << p.Odpowiedz << " | Numer: " << p.Numer << " | Identyfikator: " << p.Identyfikator << std::endl;
	return out;
}

Protocol getProtocol(std::string message)
{
	Protocol p;
	std::smatch wynik; std::string buffer;
	std::regex OP("Operacja>\\w*");
	std::regex ODP("<Odpowiedz>\\w*");
	std::regex NUM("<Numer>\\w*");
	std::regex ID("<Identyfikator>\\d*");
	std::regex TIME("<Czas>\\d{2}.\\d{2}.\\d{4}\\s\\d{2}.\\d{2}.\\d{2}");

	std::regex_search(message, wynik, OP);
	buffer = wynik[0];
	p.setOp(buffer.erase(0, 9));

	std::regex_search(message, wynik, ODP);
	buffer = wynik[0];
	p.setOdp(buffer.erase(0, 11));

	std::regex_search(message, wynik, NUM);
	buffer = wynik[0];
	p.setNum(buffer.erase(0, 7));

	std::regex_search(message, wynik, ID);
	buffer = wynik[0];
	p.setId(buffer.erase(0, 15));

	std::regex_search(message, wynik, TIME);
	buffer = wynik[0];
	p.setTime(buffer.erase(0, 6));


	return p;
}