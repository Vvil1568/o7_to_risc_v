#include "statement.h"

void AssignmentStatementContext::generateAsmCode() {
	Register* reg = var->getAssignedReg();
	expr->AssignReg(reg);
	expr->generateAsmCode();
}

void ConditionalStatementsBlock::generateAsmCode(std::string blockEndLabel, std::string endLabel) {
	if (condition != nullptr) {
		CodeGenContext::addCodeLine("#Если условие неверно - переходим к концу блока по метке " + blockEndLabel);
		if (dynamic_cast<ExprExprContext*>(condition) == nullptr) {
			condition->generateAsmCode();
			CodeGenContext::addCodeLine("beqz "+condition->getAssignedReg()->getName() + " "+blockEndLabel);
		}
		else {
			ExprExprContext* expr = dynamic_cast<ExprExprContext*>(condition);
			if (expr->getLeft()->getResType() == "REAL" || expr->getOperator()=="OR" || expr->getOperator()=="&") {
				condition->generateAsmCode();
				CodeGenContext::addCodeLine("beqz " + condition->getAssignedReg()->getName() + " " + blockEndLabel);
				if (condition->getType() != "VAR") {
					RegisterPool::getInstance().freeRegister(condition->getAssignedReg());
				}
			}
			else {
				expr->getLeft()->generateAsmCode();
				expr->getRight()->generateAsmCode();
				if (expr->getOperator() == "=") {
					CodeGenContext::addCodeLine("bne " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + blockEndLabel);
				}
				if (expr->getOperator() == "#") {
					CodeGenContext::addCodeLine("beq " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + blockEndLabel);
				}
				if (expr->getOperator() == "<") {
					CodeGenContext::addCodeLine("bge " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + blockEndLabel);
				}
				if (expr->getOperator() == ">") {
					CodeGenContext::addCodeLine("ble " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + blockEndLabel);
				}
				if (expr->getOperator() == ">=") {
					CodeGenContext::addCodeLine("blt " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + blockEndLabel);
				}
				if (expr->getOperator() == "<=") {
					CodeGenContext::addCodeLine("bgt " + expr->getLeft()->getAssignedReg()->getName() + " " + expr->getRight()->getAssignedReg()->getName() + " " + blockEndLabel);
				}
				if (expr->getLeft()->getType() != "VAR") {
					RegisterPool::getInstance().freeRegister(expr->getLeft()->getAssignedReg());
				}
				if (expr->getRight()->getType() != "VAR") {
					RegisterPool::getInstance().freeRegister(expr->getRight()->getAssignedReg());
				}
			}
		}
	}
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
}