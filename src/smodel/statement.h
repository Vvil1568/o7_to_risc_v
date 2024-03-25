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
private:
    // Переменная
    VarContext* var;
    // Значение
    ExprContext* expr;
};
#endif // STATEMENT_H