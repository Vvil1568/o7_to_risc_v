#include "codegencontext.h"

CodeGenContext::CodeGenContext() {
	code.push_back("#������ ����");
	code.push_back(".text");
	codeIndent = "    ";
	data.push_back("#������ ������");
	data.push_back(".data");
	dataIndent = "    ";
}