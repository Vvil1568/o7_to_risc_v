#ifndef REGISTERPOOL_H
#define REGISTERPOOL_H

#include <map>
#include <string>
#include <set>

//����� ������������ �� ���� ��� ��������
enum class RegType { T, S, A, FT, FS, FA };

//����� ������������ �� ���� �������
class Register {
public:
	Register(RegType type, std::string name) {
		this->type = type;
		this->name = name;
	}
	bool operator<(const Register& other) const {
		return this->name < other.name;
	}
	bool operator==(const Register& other) const {
		return this->name == other.name;
	}
	std::string getName() {
		return name;
	}
	RegType getType() {
		return type;
	}
private:
	RegType type;
	std::string name;
};

//����� ������������ �� ���� ��� ���������, �������� ������� ��������� ��������
class RegisterPool {
public:
	RegisterPool();
	//��������� ������� ������� ������� ���� �� ���� ���������
	Register takeFirstFreeReg(RegType type);
	//��������� ������� ������� � ��� ���������
	void freeRegister(Register name);
private: 
	std::map<RegType, std::set<Register>> pool;
};

#endif // REGISTERPOOL_H