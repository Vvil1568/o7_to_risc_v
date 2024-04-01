#ifndef STACKCONTROLLER_H
#define STACKCONTROLLER_H

#include <stack>
#include <string>
#include <vector>
/*
class StackFrame {
public:
	void addEntry(VarContext* var) {
		entries.push_back(entry);
	}

	int getTotalSize() { return totalSize; }

	std::vector<StackFrameEntry> getEntries() { return entries; }
private:
	int totalSize = 0;
	std::vector<StackFrameEntry> entries;
};

class StackController {
public:
	static StackController& getInstance() {
		static StackController instance;
		return instance;
	}
	void popFrame() {
		StackFrame top = stackFrames.top();
		for (StackFrameEntry entry : top.getEntries()) {
			curStackOffset += entry.varSize;
		}
		stackFrames.pop();
	}
	void popModuleFrame() {
		for (StackFrameEntry entry : moduleFrame.getEntries()) {
			curStackOffset += entry.varSize;
		}
		stackFrames.pop();
	}
	StackFrame& top() {
		return stackFrames.top();
	}
	void pushFrame() {
		stackFrames.push(StackFrame());
	}
	int addModuleVariable(std::string varName, int varSize) {
		int curOffset = curStackOffset;
		curStackOffset -= varSize;
		moduleFrame.addEntry(StackFrameEntry{ varName, varSize, curOffset });
		return curOffset;
	}
	int addVariable(std::string varName, int varSize) {
		int curOffset = curStackOffset;
		curStackOffset -= varSize;
		stackFrames.top().addEntry(StackFrameEntry{ varName, varSize, curOffset });
		return curOffset;
	}
	int getVariableAddrShift(std::string varName) {
		StackFrame top = stackFrames.top();
		for (StackFrameEntry entry : top.getEntries()) {
			if (entry.varName == varName) {
				return entry.addrShift;
			}
		}
		for (StackFrameEntry entry : moduleFrame.getEntries()) {
			if (entry.varName == varName) {
				return entry.addrShift;
			}
		}
		return 1;
	}
	StackController(StackController const&) = delete;
	void operator=(StackController const&) = delete;
private:
	StackController() {}
	int curStackOffset = 0;
	StackFrame moduleFrame;
	std::stack<StackFrame> stackFrames;
};
*/
#endif // STACKCONTROLLER_H