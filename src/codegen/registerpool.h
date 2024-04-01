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
		if (this->name.size() == other.name.size()) {
			return (this->name) < (other.name);
		}
		else {
			return (this->name).size() < (other.name).size();
		}
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

//���������� ��� �������������� ��������� ���������
struct RegisterComparator {
	bool operator()(const Register* a, const Register* b) const {
		return *a < *b; // ���������� �������� ��������, � �� ����������
	}
};

//����� ������������ �� ���� ��� ���������, �������� ������� ��������� ��������
class RegisterPool {
public:
	static RegisterPool& getInstance() {
		static RegisterPool instance;
		return instance;
	}
	static Register* a0;
	static Register* fa0;
	//��������� ������� ������� ������� ���� �� ���� ���������
	Register* takeFirstFreeReg(RegType type);
	//��������� ������� ������� � ��� ���������
	void freeRegister(Register* name);

	RegisterPool(RegisterPool const&) = delete;
	void operator=(RegisterPool const&) = delete;
private: 
	RegisterPool();
	std::map<RegType, std::set<Register*, RegisterComparator>> pool;
};

#endif // REGISTERPOOL_H