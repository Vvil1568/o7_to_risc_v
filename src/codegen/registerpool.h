#ifndef REGISTERPOOL_H
#define REGISTERPOOL_H

#include <map>
#include <string>
#include <set>

// ласс представл€ет из себ€ тип регистра
enum class RegType { T, S, A, FT, FS, FA };

// ласс представл€ет из себ€ регистр
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

// ласс представл€ет из себ€ пул регистров, содержит текущие доступные регистры
class RegisterPool {
public:
	RegisterPool();
	//ѕозвол€ет забрать регистр нужного типа из пула регистров
	Register takeFirstFreeReg(RegType type);
	//ѕозвол€ет вернуть регистр в пул регистров
	void freeRegister(Register name);
private: 
	std::map<RegType, std::set<Register>> pool;
};

#endif // REGISTERPOOL_H