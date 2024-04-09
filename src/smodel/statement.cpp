#include "statement.h"

void AssignmentStatementContext::generateAsmCode() {
	if (var != nullptr) {
		Register* reg = var->getAssignedReg();
		expr->AssignReg(reg);
		expr->generateAsmCode();
		if (var->isInMemory()) {
			var->AssignReg(expr->getAssignedReg());
			var->saveToMemory();
			expr->FreeReg();
			var->ClearReg();
		}
		else {
			if (dynamic_cast<ExprVarContext*>(expr) != nullptr && dynamic_cast<ExprVarContext*>(expr)->getVariable()->isInMemory()) {
				dynamic_cast<ExprVarContext*>(expr)->getVariable()->ClearReg();
			}
		}
	}
	else {
		expr->generateAsmCode();
	}
}

void genAsmForInvertedCond(ExprContext* condition, std::string label) {
	if (condition != nullptr) {
		CodeGenContext::addCodeComment("If the condition is not true, jump to the label " + label);
		if (dynamic_cast<ExprExprContext*>(condition) == nullptr) {
			condition->generateAsmCode();
			CodeGenContext::addCodeLine("beqz " + condition->getAssignedReg()->getName() + " " + label);
		}
		else {
			ExprExprContext* expr = dynamic_cast<ExprExprContext*>(condition);
			if (expr->getLeft()->getResType() == "REAL" || expr->getOperator() == "OR" || expr->getOperator() == "&") {
				condition->generateAsmCode();
				CodeGenContext::addCodeLine("beqz " + condition->getAssignedReg()->getName() + " " + label);
				condition->FreeReg();
			}
			else {
				expr->getLeft()->generateAsmCode();
				expr->getRight()->generateAsmCode();
				if (expr->getOperator() == "=") {
					CodeGenContext::addCodeLine("bne " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + label);
				}
				if (expr->getOperator() == "#") {
					CodeGenContext::addCodeLine("beq " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + label);
				}
				if (expr->getOperator() == "<") {
					CodeGenContext::addCodeLine("bge " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + label);
				}
				if (expr->getOperator() == ">") {
					CodeGenContext::addCodeLine("ble " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + label);
				}
				if (expr->getOperator() == ">=") {
					CodeGenContext::addCodeLine("blt " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + label);
				}
				if (expr->getOperator() == "<=") {
					CodeGenContext::addCodeLine("bgt " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + label);
				}
				if (expr->getLeft()->getType() != "VAR" || static_cast<ExprVarContext*>(expr->getLeft())->getVariable()->isInMemory()) {
					expr->getLeft()->FreeReg();
				}
				if (expr->getRight()->getType() != "VAR" || static_cast<ExprVarContext*>(expr->getRight())->getVariable()->isInMemory()) {
					expr->getRight()->FreeReg();
				}
			}
		}
	}
}

void ConditionalStatementsBlock::generateAsmCode(std::string blockEndLabel, std::string endLabel) {
	genAsmForInvertedCond(condition, blockEndLabel);
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	CodeGenContext::addCodeComment("Jump to the end of the if statement");
	CodeGenContext::addCodeLine("b " + endLabel);
	CodeGenContext::addCodeComment("If block end label");
	CodeGenContext::addCodeLine(blockEndLabel+":");
}

void IfStatementContext::generateAsmCode() {
	CodeGenContext::addCodeComment("IF statement beginning");
	std::string ifLabel = CodeGenContext::pushContext("if");
	int i = 0;
	for (ConditionalStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeComment("Beginning of the if block number "+std::to_string(i));
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(ifLabel+"_block"+std::to_string(i), ifLabel + "_end");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeComment("Ending of the if block number " + std::to_string(i));
	}
	CodeGenContext::addCodeComment("If statement ending label");
	CodeGenContext::addCodeLine(ifLabel + "_end:");
	CodeGenContext::addCodeComment("IF statement ending");
	CodeGenContext::popContext();
}

void CaseStatementContext::generateAsmCode() {
	CodeGenContext::addCodeComment("Case statement beginning");
	std::string caseLabel = CodeGenContext::pushContext("case");
	int i = 0;
	for (ConditionalStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeComment("Beginning of the case block number " + std::to_string(i));
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(caseLabel + "_block" + std::to_string(i), caseLabel + "_end");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeComment("Ending of the case block number  " + std::to_string(i));
	}
	CodeGenContext::addCodeComment("Case statement ending label");
	CodeGenContext::addCodeLine(caseLabel + "_end:");
	CodeGenContext::addCodeComment("Case statement ending");
	CodeGenContext::popContext();
}

void WhileStatementsBlock::generateAsmCode(std::string blockEndLabel, std::string begLabel) {
	genAsmForInvertedCond(condition, blockEndLabel);
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	CodeGenContext::addCodeComment("Jump to the beginning of the while statement");
	CodeGenContext::addCodeLine("b " + begLabel);
	CodeGenContext::addCodeComment("While block end label");
	CodeGenContext::addCodeLine(blockEndLabel + ":");
}

void WhileStatementContext::generateAsmCode() {
	CodeGenContext::addCodeComment("While statement beginning");
	std::string whileLabel = CodeGenContext::pushContext("while");
	CodeGenContext::addCodeComment("While statement beginning label");
	CodeGenContext::addCodeLine(whileLabel + "_beg:");
	int i = 0;
	for (WhileStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeComment("Beginning of the while block number " + std::to_string(i));
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(whileLabel + "_block" + std::to_string(i), whileLabel + "_beg");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeComment("Ending of the while block number " + std::to_string(i));
	}
	
	CodeGenContext::addCodeComment("While statement ending");
	CodeGenContext::popContext();
}

void RepeatStatementContext::generateAsmCode() {
	CodeGenContext::addCodeComment("Repeat statement beginning");
	std::string repeatLabel = CodeGenContext::pushContext("repeat");
	CodeGenContext::addCodeComment("Repeat statement beginning label");
	CodeGenContext::addCodeLine(repeatLabel + "_beg:");
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	genAsmForInvertedCond(condition, repeatLabel + "_beg");
	CodeGenContext::addCodeComment("Repeat statement ending");
	CodeGenContext::popContext();
}

void ForStatementContext::generateAsmCode() { 
	CodeGenContext::addCodeComment("For statement beginning");
	CodeGenContext::addCodeComment("Loop initial statement");
	init->generateAsmCode();
	std::string forLabel = CodeGenContext::pushContext("for");
	CodeGenContext::addCodeComment("For statement beginning label");
	CodeGenContext::addCodeLine(forLabel + "_beg:");
	genAsmForInvertedCond(condition, forLabel + "_end");
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	step->generateAsmCode();
	CodeGenContext::addCodeComment("Jumping to the beginning of the for statement");
	CodeGenContext::addCodeLine("b " + forLabel + "_beg");
	CodeGenContext::addCodeComment("For statement ending label");
	CodeGenContext::addCodeLine(forLabel + "_end:");
	CodeGenContext::addCodeComment("For statement ending");
	CodeGenContext::popContext();
}

void SysCallStatementContext::generateAsmCode() {
	CodeGenContext::addCodeComment("Loading the system call number into a7 register");
	CodeGenContext::addCodeLine("li a7 " + std::to_string(sysCallNum));
	CodeGenContext::addCodeComment("Invoking the system call");
	CodeGenContext::addCodeLine("ecall");
}

void ManualStatementContext::generateAsmCode() {
	for (std::string asmLine : asmCode) {
		CodeGenContext::addCodeLine(asmLine);
	}
}

