#ifndef REGISTERPOOL_H
#define REGISTERPOOL_H

#include <map>
#include <string>
#include <set>
#include <stack>

//����� ������������ �� ���� ��� ��������
enum class RegType { T, S, A, FT, FS, FA, AR, FAR };

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
	static Register* a1;
	static Register* fa1;
	//��������� ������� ������� ������� ���� �� ���� ���������
	Register* takeFirstFreeReg(RegType type);
	void takeReg(Register* reg) {
		pool[reg->getType()].erase(reg);
		reversedPool[reg->getType()].insert(reg);
	}
	//��������� ������� ������� � ��� ���������
	void freeRegister(Register* name);
	//��������� ������, ������� ��������� ������� ���� ���� � ����
	int getRegCount(RegType type);
	void resetToFree(RegType type) {
		pool[type] = freePools[type];
	}
	void pushState(RegType type) {
		poolStacks[type].push(pool[type]);
	}
	void popState(RegType type) {
		pool[type] = poolStacks[type].top();
		poolStacks[type].pop();
	}
	std::set<Register*, RegisterComparator> getReversedPool(RegType type) {
		return reversedPool[type];
	}


	RegisterPool(RegisterPool const&) = delete;
	void operator=(RegisterPool const&) = delete;
private: 
	RegisterPool();
	std::map<RegType, std::set<Register*, RegisterComparator>> freePools;
	std::map<RegType, std::stack<std::set<Register*, RegisterComparator>>> poolStacks;
	std::map<RegType, std::set<Register*, RegisterComparator>> pool;
	std::map<RegType, std::set<Register*, RegisterComparator>> reversedPool;
};

#endif // REGISTERPOOL_H