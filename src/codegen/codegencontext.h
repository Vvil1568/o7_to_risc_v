#ifndef CODEGENCONTEXT_H
#define CODEGENCONTEXT_H

#include <vector>
#include <string>

class CodeGenContext {
public:
	static CodeGenContext& getInstance() {
		static CodeGenContext instance;
		return instance;
	}

	static void addCodeLine(std::string line) {
		getInstance().code.push_back(getInstance().codeIndent+line);
	}
	static void addDataLine(std::string line) {
		getInstance().data.push_back(getInstance().dataIndent + line);
	}
	static std::string getStrLitLabel() {
		getInstance().strLitCount++;
		return "literal_str_" + std::to_string(getInstance().strLitCount);
	}

	std::vector<std::string> code;
	std::vector<std::string> data;
	std::string dataIndent;
	std::string codeIndent;
private:
	int strLitCount = 0;
	CodeGenContext();
};

#endif // CODEGENCONTEXT_H