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
private:
    // ����������
    VarContext* var;
    // ��������
    ExprContext* expr;
};
#endif // STATEMENT_H