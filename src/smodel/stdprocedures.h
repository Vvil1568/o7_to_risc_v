#ifndef STDPROCEDURES_H
#define STDPROCEDURES_H

#include "procedure.h"
#include <string>
#include <map>

//Класс представляет из себя пул регистров, содержит текущие доступные регистры
class STDProcedures {
public:
	static STDProcedures& getInstance() {
		static STDProcedures instance;
		return instance;
	}

	std::map<std::string, ProcContext*> getAllProcs() {
		return procList;
	}

	std::vector<std::pair<std::string, ProcContext*>> getProcsByName(std::string name) {
		std::vector<std::pair<std::string, ProcContext*>> res;
		for (auto entry : procList) {
			if (entry.first.substr(0, name.size()) == name) {
				res.push_back(entry);
			}
		}
		return res;
	}

	STDProcedures(STDProcedures const&) = delete;
	void operator=(STDProcedures const&) = delete;
private:
	STDProcedures();
	std::map<std::string, ProcContext*> procList;
};

#endif // STDPROCEDURES_H