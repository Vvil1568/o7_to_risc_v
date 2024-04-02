#ifndef STATEMENT_H
#define STATEMENT_H

#include "context.h"
#include "variable.h"
#include "expr.h"

// Класс, определяющий контекст выражения.
class StatementContext : public Context {
public:
    // Вывод отладочной информации о контексте выражения
    virtual void debugOut() {
        std::cout << "STATEMENT";
    }

    // Информации о типе выражения
    virtual std::string getType() {
        return "UNDEFINED";
    }

    virtual void generateAsmCode() {}
};

// Класс, определяющий контекст выражения присвоения.
class AssignmentStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    AssignmentStatementContext(VarContext* var, ExprContext* expr) {
        this->var = var;
        this->expr = expr;
    }
    
    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "ASSIGNMENT";
    }
    // Информации о типе выражения
    virtual std::string getType() {
        return "ASSIGNMENT";
    }

    virtual void generateAsmCode();
private:
    // Переменная
    VarContext* var;
    // Значение
    ExprContext* expr;
};

// Класс, определяющий контекст условного блока.
class ConditionalStatementsBlock {
public:
    // Создание выражения присвоения
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

// Класс, определяющий контекст условного выражения.
class IfStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    IfStatementContext(std::vector<ConditionalStatementsBlock> statementBlocks) {
        this->statementBlocks = statementBlocks;
    }

    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "IF";
    }
    // Информации о типе выражения
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

// Класс, определяющий контекст условного выражения.
class CaseStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    CaseStatementContext(std::vector<ConditionalStatementsBlock> statementBlocks) {
        this->statementBlocks = statementBlocks;
    }

    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "CASE";
    }
    // Информации о типе выражения
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

// Класс, определяющий контекст цикличного блока while.
class WhileStatementsBlock {
public:
    // Создание выражения присвоения
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

// Класс, определяющий контекст оператора while.
class WhileStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    WhileStatementContext(std::vector<WhileStatementsBlock> statementBlocks) {
        this->statementBlocks = statementBlocks;
    }

    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "WHILE";
    }
    // Информации о типе выражения
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

// Класс, определяющий контекст оператора repeat.
class RepeatStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    RepeatStatementContext(ExprContext* condition, std::vector<StatementContext*> statements) {
        this->condition = condition;
        this->statements = statements;
    }

    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "REPEAT";
    }
    // Информации о типе выражения
    virtual std::string getType() {
        return "REPEAT";
    }

    virtual void generateAsmCode();
private:
    ExprContext* condition = nullptr;
    std::vector<StatementContext*> statements;
};

// Класс, определяющий контекст оператора for.
class ForStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    ForStatementContext(StatementContext* init, ExprContext* condition, StatementContext* step, std::vector<StatementContext*> statements) {
        this->init = init;
        this->condition = condition;
        this->step = step;
        this->statements = statements;
    }

    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "FOR";
    }
    // Информации о типе выражения
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

// Класс, определяющий контекст системного вызова.
class SysCallStatementContext : public StatementContext {
public:
    // Создание выражения присвоения
    SysCallStatementContext(int sysCallNum) {
        this->sysCallNum = sysCallNum;
    }

    // Вывод отладочной информации о выражении присвоения
    virtual void debugOut() {
        std::cout << "SysCall";
    }
    // Информации о типе выражения
    virtual std::string getType() {
        return "SysCall";
    }

    virtual void generateAsmCode();
private:
    int sysCallNum;
};
#endif // STATEMENT_H