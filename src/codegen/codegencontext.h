#ifndef CODEGENCONTEXT_H
#define CODEGENCONTEXT_H

#include <vector>
#include <string>
#include <stack>

//Структура описывает кадр стека контекста
struct ContextStackFrame {
	std::string name;
	int ifCount = 0;
	int caseCount = 0;
	int forCount = 0;
	int whileCount = 0;
};

//Контекст кодогенератора
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
	static std::string pushContext(std::string name) {
		if (name == "if" || name == "case" || name == "for" || name == "while") {
			ContextStackFrame top = getInstance().contextStack.back();
			if (name == "if") {
				top.ifCount++;
				name += std::to_string(top.ifCount);
			}
			if (name == "case") {
				top.caseCount++;
				name += std::to_string(top.ifCount);
			}
			if (name == "for") {
				top.forCount++;
				name += std::to_string(top.ifCount);
			}
			if (name == "while") {
				top.whileCount++;
				name += std::to_string(top.ifCount);
			}
		}
		getInstance().contextStack.push_back(ContextStackFrame { name, 0, 0, 0, 0 });
		std::string labelName = "";
		bool first = true;
		for (auto it = getInstance().contextStack.begin(); it != getInstance().contextStack.end(); it++) {
			if (!first) {
				labelName += "_";
			}
			first = false;
			labelName += (*it).name;
		}
		return labelName;
	}
	static void popContext() {
		getInstance().contextStack.pop_back();
	}
	std::vector<std::string> code;
	std::vector<std::string> data;
	std::string dataIndent;
	std::string codeIndent;
private:
	std::vector<ContextStackFrame> contextStack;
	int strLitCount = 0;
	CodeGenContext();
};

#endif // CODEGENCONTEXT_H