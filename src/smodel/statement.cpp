#include "statement.h"

void AssignmentStatementContext::generateAsmCode() {
	if (var != nullptr) {
		Register* reg = var->getAssignedReg();
		expr->AssignReg(reg);
		expr->generateAsmCode();
		if (var->isOnStack()) {
			CodeGenContext::addCodeLine("#Уберем значение на стек по адресу sp" + std::to_string(var->getStackOffset()));
			CodeGenContext::addCodeLine("sw " + expr->getAssignedReg()->getName() + " " + std::to_string(var->getStackOffset()) + "(sp)");
		}
	}
	else {
		expr->generateAsmCode();
	}
}

void genAsmForInvertedCond(ExprContext* condition, std::string label) {
	if (condition != nullptr) {
		CodeGenContext::addCodeLine("#Если условие неверно - переходим по метке " + label);
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
	CodeGenContext::addCodeLine("#Переходим к концу условия");
	CodeGenContext::addCodeLine("b " + endLabel);
	CodeGenContext::addCodeLine("#Метка конца условного блока");
	CodeGenContext::addCodeLine(blockEndLabel+":");
}

void IfStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#Начало условия");
	std::string ifLabel = CodeGenContext::pushContext("if");
	int i = 0;
	for (ConditionalStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeLine("#Начало "+std::to_string(i)+"-го условного блока");
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(ifLabel+"_block"+std::to_string(i), ifLabel + "_end");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeLine("#Конец " + std::to_string(i) + "-го условного блока");
	}
	CodeGenContext::addCodeLine("#Метка конца условия");
	CodeGenContext::addCodeLine(ifLabel + "_end:");
	CodeGenContext::addCodeLine("#Конец условия");
	CodeGenContext::popContext();
}

void CaseStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#Начало оператора case");
	std::string caseLabel = CodeGenContext::pushContext("case");
	int i = 0;
	for (ConditionalStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeLine("#Начало " + std::to_string(i) + "-го case-блока");
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(caseLabel + "_block" + std::to_string(i), caseLabel + "_end");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeLine("#Конец " + std::to_string(i) + "-го case-блока");
	}
	CodeGenContext::addCodeLine("#Метка конца оператора case");
	CodeGenContext::addCodeLine(caseLabel + "_end:");
	CodeGenContext::addCodeLine("#Конец оператора case");
	CodeGenContext::popContext();
}

void WhileStatementsBlock::generateAsmCode(std::string blockEndLabel, std::string begLabel) {
	genAsmForInvertedCond(condition, blockEndLabel);
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	CodeGenContext::addCodeLine("#Переходим к началу оператора while");
	CodeGenContext::addCodeLine("b " + begLabel);
	CodeGenContext::addCodeLine("#Метка конца while-блока");
	CodeGenContext::addCodeLine(blockEndLabel + ":");
}

void WhileStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#Начало цикла while");
	std::string whileLabel = CodeGenContext::pushContext("while");
	CodeGenContext::addCodeLine("#Метка начала цикла while");
	CodeGenContext::addCodeLine(whileLabel + "_beg:");
	int i = 0;
	for (WhileStatementsBlock statement : statementBlocks) {
		i++;
		CodeGenContext::addCodeLine("#Начало " + std::to_string(i) + "-го while-блока");
		CodeGenContext::getInstance().codeIndent += "    ";
		statement.generateAsmCode(whileLabel + "_block" + std::to_string(i), whileLabel + "_beg");
		CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
		CodeGenContext::addCodeLine("#Конец " + std::to_string(i) + "-го while-блока");
	}
	
	CodeGenContext::addCodeLine("#Конец цикла while");
	CodeGenContext::popContext();
}

void RepeatStatementContext::generateAsmCode() {
	CodeGenContext::addCodeLine("#Начало цикла repeat");
	std::string repeatLabel = CodeGenContext::pushContext("repeat");
	CodeGenContext::addCodeLine("#Метка начала цикла repeat");
	CodeGenContext::addCodeLine(repeatLabel + "_beg:");
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	genAsmForInvertedCond(condition, repeatLabel + "_beg");
	CodeGenContext::addCodeLine("#Конец цикла repeat");
	CodeGenContext::popContext();
}

void ForStatementContext::generateAsmCode() { 
	CodeGenContext::addCodeLine("#Начало цикла for");
	CodeGenContext::addCodeLine("#Начальное выражение цикла");
	init->generateAsmCode();
	std::string forLabel = CodeGenContext::pushContext("for");
	CodeGenContext::addCodeLine("#Метка начала цикла for");
	CodeGenContext::addCodeLine(forLabel + "_beg:");
	genAsmForInvertedCond(condition, forLabel + "_end");
	for (StatementContext* statement : statements) {
		statement->generateAsmCode();
	}
	step->generateAsmCode();
	CodeGenContext::addCodeLine("#Переходим к началу оператора while");
	CodeGenContext::addCodeLine("b " + forLabel + "_beg");
	CodeGenContext::addCodeLine("#Метка конца цикла for");
	CodeGenContext::addCodeLine(forLabel + "_end:");
	CodeGenContext::addCodeLine("#Конец цикла for");
	CodeGenContext::popContext();
}