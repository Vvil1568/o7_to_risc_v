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

// �����, ������������ �������� ���������� ����� while.
class WhileStatementsBlock {
public:
    // �������� ��������� ����������
    WhileStatementsBlock() {}
    void setCondition(ExprContext* condition) {
        this->condition = condition;
    }
    void setStatements(std::vector<StatementContext*> statements) {
        this->statements = statements;
    }
    void generateAsmCode(std::string blockEndLabel, std::string begLabel);
private:
    ExprContext* condition = nullptr;
    std::vector<StatementContext*> statements;
};

// �����, ������������ �������� ��������� while.
class WhileStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    WhileStatementContext(std::vector<WhileStatementsBlock> statementBlocks) {
        this->statementBlocks = statementBlocks;
    }

    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "WHILE";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "WHILE";
    }

    void addLoopBlock(WhileStatementsBlock block) {
        statementBlocks.push_back(block);
    }

    virtual void generateAsmCode();
private:
    std::vector<WhileStatementsBlock> statementBlocks;
};

// �����, ������������ �������� ��������� repeat.
class RepeatStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    RepeatStatementContext(ExprContext* condition, std::vector<StatementContext*> statements) {
        this->condition = condition;
        this->statements = statements;
    }

    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "REPEAT";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "REPEAT";
    }

    virtual void generateAsmCode();
private:
    ExprContext* condition = nullptr;
    std::vector<StatementContext*> statements;
};

// �����, ������������ �������� ��������� for.
class ForStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    ForStatementContext(StatementContext* init, ExprContext* condition, StatementContext* step, std::vector<StatementContext*> statements) {
        this->init = init;
        this->condition = condition;
        this->step = step;
        this->statements = statements;
    }

    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "FOR";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "FOR";
    }

    virtual void generateAsmCode();
private:
    StatementContext* init = nullptr;
    ExprContext* condition = nullptr;
    StatementContext* step = nullptr;
    std::vector<StatementContext*> statements;
};

// �����, ������������ �������� ���������� ������.
class SysCallStatementContext : public StatementContext {
public:
    // �������� ��������� ����������
    SysCallStatementContext(int sysCallNum) {
        this->sysCallNum = sysCallNum;
    }

    // ����� ���������� ���������� � ��������� ����������
    virtual void debugOut() {
        std::cout << "SysCall";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "SysCall";
    }

    virtual void generateAsmCode();
private:
    int sysCallNum;
};
#endif // STATEMENT_H