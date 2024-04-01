#ifndef REGISTERPOOL_H
#define REGISTERPOOL_H

#include <map>
#include <string>
#include <set>

//Класс представляет из себя тип регистра
enum class RegType { T, S, A, FT, FS, FA };

//Класс представляет из себя регистр
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

//Компаратор для упорядоченного множества регистров
struct RegisterComparator {
	bool operator()(const Register* a, const Register* b) const {
		return *a < *b; // Сравниваем значения объектов, а не указателей
	}
};

//Класс представляет из себя пул регистров, содержит текущие доступные регистры
class RegisterPool {
public:
	static RegisterPool& getInstance() {
		static RegisterPool instance;
		return instance;
	}
	static Register* a0;
	static Register* fa0;
	//Позволяет забрать регистр нужного типа из пула регистров
	Register* takeFirstFreeReg(RegType type);
	//Позволяет вернуть регистр в пул регистров
	void freeRegister(Register* name);

	RegisterPool(RegisterPool const&) = delete;
	void operator=(RegisterPool const&) = delete;
private: 
	RegisterPool();
	std::map<RegType, std::set<Register*, RegisterComparator>> pool;
};

#endif // REGISTERPOOL_H