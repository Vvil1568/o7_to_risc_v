#ifndef CODEGENCONTEXT_H
#define CODEGENCONTEXT_H

#include <vector>
#include <string>
#include <stack>
#include <set>

//Структура описывает кадр стека контекста
struct ContextStackFrame {
	std::string name;
	int ifCount = 0;
	int caseCount = 0;
	int forCount = 0;
	int whileCount = 0;
	int repeatCount = 0;
};

//Контекст кодогенератора
class CodeGenContext {
public:
	
	static CodeGenContext& getInstance() {
		static CodeGenContext instance;
		return instance;
	}
	bool enableComments = true;
	static void addCodeLine(std::string line) {
		getInstance().code.push_back(getInstance().codeIndent + line);
	}
	static void addDataLine(std::string line) {
		getInstance().data.push_back(getInstance().dataIndent + line);
	}
	static void addCodeComment(std::string line) {
		if(getInstance().enableComments)
			addCodeLine("#" + line);
	}
	static void addDataComment(std::string line) {
		if (getInstance().enableComments)
			addDataLine("#" + line);
	}
	static std::string getStrLitLabel() {
		getInstance().strLitCount++;
		return "literal_str_" + std::to_string(getInstance().strLitCount);
	}
	static bool adRealLitLabel(std::string label) {
		int size = getInstance().realLabels.size();
		getInstance().realLabels.insert(label);
		return getInstance().realLabels.size()>size;
	}


	static std::string getLabelName(std::string name) {
		std::string labelName = "";
		bool first = true;
		for (auto it = getInstance().contextStack.begin(); it != getInstance().contextStack.end(); it++) {
			if (!first) {
				labelName += "_";
			}
			first = false;
			labelName += (*it).name;
		}
		return labelName+"_"+name;
	}

	static std::string pushContext(std::string name) {
		if (name == "if" || name == "case" || name == "for" || name == "while" || name == "repeat") {
			ContextStackFrame& top = getInstance().contextStack.back();
			if (name == "if") {
				top.ifCount++;
				name += std::to_string(top.ifCount);
			}
			if (name == "case") {
				top.caseCount++;
				name += std::to_string(top.caseCount);
			}
			if (name == "for") {
				top.forCount++;
				name += std::to_string(top.forCount);
			}
			if (name == "while") {
				top.whileCount++;
				name += std::to_string(top.whileCount);
			}
			if (name == "repeat") {
				top.repeatCount++;
				name += std::to_string(top.repeatCount);
			}
		}
		getInstance().contextStack.push_back(ContextStackFrame{ name, 0, 0, 0, 0 });
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
	static int pushStack(int size) {
		int res = getInstance().stackOffset;
		getInstance().stackOffset -= size;
		return res;
	}
	static void popStack(int size) {
		getInstance().stackOffset += size;
	}
	std::vector<std::string> code;
	std::vector<std::string> data;
	std::string dataIndent;
	std::string codeIndent;
	std::set<std::string> realLabels;
	bool isInProc;
private:
	std::vector<ContextStackFrame> contextStack;
	int strLitCount = 0;
	int stackOffset = 0;
	CodeGenContext();
};

#endif // CODEGENCONTEXT_H