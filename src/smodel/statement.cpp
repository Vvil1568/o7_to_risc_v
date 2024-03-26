#include "statement.h"

void AssignmentStatementContext::generateAsmCode() {
	Register* reg = var->getAssignedReg();
	expr->AssignReg(reg);
	expr->generateAsmCode();
}

