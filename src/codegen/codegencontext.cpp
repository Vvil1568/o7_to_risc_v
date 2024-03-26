#include "codegencontext.h"

CodeGenContext::CodeGenContext() {
	code.push_back("#Секция кода");
	code.push_back(".text");
	codeIndent = "    ";
	data.push_back("#Секция данных");
	data.push_back(".data");
	dataIndent = "    ";
}