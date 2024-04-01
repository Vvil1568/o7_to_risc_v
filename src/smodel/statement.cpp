#include "statement.h"

void AssignmentStatementContext::generateAsmCode() {
	if (var != nullptr) {
		Register* reg = var->getAssignedReg();
		expr->AssignReg(reg);
		expr->generateAsmCode();
		if (var->isOnStack()) {
			CodeGenContext::addCodeLine("#������ �������� �� ���� �� ������ sp" + std::to_string(var->getStackOffset()));
			CodeGenContext::addCodeLine("sw " + expr->getAssignedReg()->getName() + " " + std::to_string(var->getStackOffset()) + "(sp)");
		}
	}
	else {
		expr->generateAsmCode();
	}
}

void genAsmForInvertedCond(ExprContext* condition, std::string label) {
	if (condition != nullptr) {
		CodeGenContext::addCodeLine("#���� ������� ������� - ��������� �� ����� " + label);
		if (dynamic_cast<ExprExprContext*>(condition) == nullptr) {
			condition->generateAsmCode();
			CodeGenContext::addCodeLine("beqz " + condition->getAssignedReg()->getName() + " " + label);
		}
		else {
			ExprExprContext* expr = dynamic_cast<ExprExprContext*>(condition);
			if (expr->getLeft()->getResType() == "REAL" || expr->getOperator() == "OR" || expr->getOperator() == "&") {
				condition->generateAsmCode();
				CodeGenContext::addCodeLine("beqz " + condition->getAssignedReg()->getName() + " " + label);
				if (condition->getType() != "VAR") {
					RegisterPool::getInstance().freeRegister(condition->getAssignedReg());
				}
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
				if (expr->getLeft()->getType() != "VAR" || static_cast<ExprVarContext*>(expr->getLeft())->getVariable()->isOnStack()) {
					RegisterPool::getInstance().freeRegister(expr->getLeft()->getAssignedReg());
				}
				if (expr->getRight()->getType() != "VAR" || static_cast<ExprVarContext*>(expr->getRight())->getVariable()->isOnStack()) {
					RegisterPool::getInstance().freeRegister(expr->getRight()->getAssignedReg());
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
	CodeGenContext::addCodeLine("#��������� � ����� �������");
	CodeGenContext::addCodeLine("b " + endLabel);
	CodeGenContext::addCodeLine("#����� ����� ��������� �����");
	CodeGenContext::addCodeLine(blockEndLabel+":");
}

void IfStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#������ �������");
	std::string ifLabel = CodeGenContext::pushContext("if");
	int i = 0;
	for (ConditionalStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeLine("#������ "+std::to_string(i)+"-�� ��������� �����");
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(ifLabel+"_block"+std::to_string(i), ifLabel + "_end");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeLine("#����� " + std::to_string(i) + "-�� ��������� �����");
	}
	CodeGenContext::addCodeLine("#����� ����� �������");
	CodeGenContext::addCodeLine(ifLabel + "_end:");
	CodeGenContext::addCodeLine("#����� �������");
	CodeGenContext::popContext();
}

void CaseStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#������ ��������� case");
	std::string caseLabel = CodeGenContext::pushContext("case");
	int i = 0;
	for (ConditionalStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeLine("#������ " + std::to_string(i) + "-�� case-�����");
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(caseLabel + "_block" + std::to_string(i), caseLabel + "_end");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeLine("#����� " + std::to_string(i) + "-�� case-�����");
	}
	CodeGenContext::addCodeLine("#����� ����� ��������� case");
	CodeGenContext::addCodeLine(caseLabel + "_end:");
	CodeGenContext::addCodeLine("#����� ��������� case");
	CodeGenContext::popContext();
}

void WhileStatementsBlock::generateAsmCode(std::string blockEndLabel, std::string begLabel) {
	genAsmForInvertedCond(condition, blockEndLabel);
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	CodeGenContext::addCodeLine("#��������� � ������ ��������� while");
	CodeGenContext::addCodeLine("b " + begLabel);
	CodeGenContext::addCodeLine("#����� ����� while-�����");
	CodeGenContext::addCodeLine(blockEndLabel + ":");
}

void WhileStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#������ ����� while");
	std::string whileLabel = CodeGenContext::pushContext("while");
	CodeGenContext::addCodeLine("#����� ������ ����� while");
	CodeGenContext::addCodeLine(whileLabel + "_beg:");
	int i = 0;
	for (WhileStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeLine("#������ " + std::to_string(i) + "-�� while-�����");
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(whileLabel + "_block" + std::to_string(i), whileLabel + "_beg");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeLine("#����� " + std::to_string(i) + "-�� while-�����");
	}
	
	CodeGenContext::addCodeLine("#����� ����� while");
	CodeGenContext::popContext();
}

void RepeatStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#������ ����� repeat");
	std::string repeatLabel = CodeGenContext::pushContext("repeat");
	CodeGenContext::addCodeLine("#����� ������ ����� repeat");
	CodeGenContext::addCodeLine(repeatLabel + "_beg:");
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	genAsmForInvertedCond(condition, repeatLabel + "_beg");
	CodeGenContext::addCodeLine("#����� ����� repeat");
	CodeGenContext::popContext();
}

void ForStatementContext::generateAsmCode() { 
	CodeGenContext::addCodeLine("#������ ����� for");
	CodeGenContext::addCodeLine("#��������� ��������� �����");
	init->generateAsmCode();
	std::string forLabel = CodeGenContext::pushContext("for");
	CodeGenContext::addCodeLine("#����� ������ ����� for");
	CodeGenContext::addCodeLine(forLabel + "_beg:");
	genAsmForInvertedCond(condition, forLabel + "_end");
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	step->generateAsmCode();
	CodeGenContext::addCodeLine("#��������� � ������ ��������� while");
	CodeGenContext::addCodeLine("b " + forLabel + "_beg");
	CodeGenContext::addCodeLine("#����� ����� ����� for");
	CodeGenContext::addCodeLine(forLabel + "_end:");
	CodeGenContext::addCodeLine("#����� ����� for");
	CodeGenContext::popContext();
}