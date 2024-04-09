#include "codegencontext.h"

CodeGenContext::CodeGenContext() {
	if(enableComments)
		code.push_back("#Code segment");
	code.push_back(".text");
	codeIndent = "    ";
	if (enableComments)
		data.push_back("#Data segment");
	data.push_back(".data");
	dataIndent = "    ";
}