#ifndef STATEMENT_H
#define STATEMENT_H

#include "context.h"
#include "variable.h"
#include "expr.h"

// �����, ������������ �������� ���������.
class StatementContext : public Context {
public:
    // ����� ���������� ���������� � ��������� ���������
    virtual void debugOut() {
        std::cout << "STATEMENT";
    }

    // ���������� � ���� ���������
    virtual std::string getType() {
        return "UNDEFINED";
    }

    virtual void generateAsmCode() {}
};

// �����, ������������ �������� ��������� ����������.
class AssignmentStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    AssignmentStatementContext(VarContext* var, ExprContext* expr) {
        this->var = var;
        this->expr = expr;
    }
    
    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "ASSIGNMENT";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "ASSIGNMENT";
    }

    virtual void generateAsmCode();
private:
    // ����������
    VarContext* var;
    // ��������
    ExprContext* expr;
};

// �����, ������������ �������� ��������� �����.
class ConditionalStatementsBlock {
public:
    // �������� ��������� ����������
    ConditionalStatementsBlock() {}
    void setCondition(ExprContext* condition) {
        this->condition = condition;
    }
    void setStatements(std::vector<StatementContext*> statements) {
        this->statements = statements;
    }
    void generateAsmCode(std::string blockEndLabel, std::string endLabel);
private:
    ExprContext* condition = nullptr;
    std::vector<StatementContext*> statements;
};

// �����, ������������ �������� ��������� ���������.
class IfStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    IfStatementContext(std::vector<ConditionalStatementsBlock> statementBlocks) {
        this->statementBlocks = statementBlocks;
    }

    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "IF";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "IF";
    }

    void addCondBlock(ConditionalStatementsBlock block) {
        statementBlocks.push_back(block);
    }

    virtual void generateAsmCode();
private:
    std::vector<ConditionalStatementsBlock> statementBlocks;
};

// �����, ������������ �������� ��������� ���������.
class CaseStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    CaseStatementContext(std::vector<ConditionalStatementsBlock> statementBlocks) {
        this->statementBlocks = statementBlocks;
    }

    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "CASE";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "CASE";
    }

    void addCondBlock(ConditionalStatementsBlock block) {
        statementBlocks.push_back(block);
    }

    virtual void generateAsmCode();
private:
    std::vector<ConditionalStatementsBlock> statementBlocks;
};
#endif // STATEMENT_H