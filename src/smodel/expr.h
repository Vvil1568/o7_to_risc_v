#ifndef EXPR_H
#define EXPR_H

#include <iostream>
#include <string>
#include <vector>

#include "type.h"
#include "context.h"
#include "variable.h"
#include "const.h"
#include "registerpool.h"
#include "codegencontext.h"
#include <algorithm>
#include "stackcontroller.h"

// �����, ������������ �������� ���������.
// ��������������, ��� � ����� ���������� �������� �������� �����
class ExprContext : public Context {
protected:
    Register* assignedRegister = nullptr;
public:
    // ����� ���������� ���������� � ��������� �������
    virtual void debugOut() {

    }

    virtual std::string getResType() {
        return getType();
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "UNDEFINED";
    }
    virtual void AssignReg(Register* reg) {
        assignedRegister = reg;
    }
    virtual void generateAsmCode() = 0;

    virtual Register* getAssignedReg() {
        if (assignedRegister == nullptr) {
            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
        }
        return assignedRegister;
    }
};

// �����, ������������ �������� �������������� ��������.
class ExprIntContext : public ExprContext {
public:
    // �������� �������������� ��������
    ExprIntContext(int v) : value(v) { }

    // ����� ���������� ���������� � ������������� ��������
    virtual void debugOut() {
        std::cout << value;
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "INT";
    }
    int getIntValue() {
        return value;
    }
    virtual void generateAsmCode();
private:
    // �������� ��������
    int value;
};

// �����, ������������ �������� ������������� ��������.
class ExprRealContext : public ExprContext {
public:
    // �������� ������������� ��������
    ExprRealContext(double v) : value(v) { }

    // ����� ���������� ���������� � ������������ ��������
    virtual void debugOut() {
        std::cout << value;
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "REAL";
    }
    double getRealValue() {
        return value;
    }
    virtual void generateAsmCode();

    virtual Register* getAssignedReg() {
        if (assignedRegister == nullptr) {
            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
        }
        return assignedRegister;
    }
private:
    // �������� ��������
    double value;
};

// �����, ������������ �������� �������� ��������.
class ExprBooleanContext : public ExprContext {
public:
    // �������� �������� ��������
    ExprBooleanContext(bool v) : value(v) { }

    // ����� ���������� ���������� � ������� ��������
    virtual void debugOut() {
        std::cout << value;
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "BOOL";
    }
    bool getBoolValue() {
        return value;
    }
    virtual void generateAsmCode();
private:
    // �������� ���������
    bool value;
};

// �����, ������������ �������� ���������� ��������.
class ExprStringContext : public ExprContext {
public:
    // �������� ���������� ��������
    ExprStringContext(std::string v) : value(v) { }

    // ����� ���������� ���������� � ��������� ��������
    virtual void debugOut() {
        std::cout << value;
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "ARRAY OF CHAR";
    }
    std::string getStrValue() {
        return value;
    }
    virtual void generateAsmCode();
private:
    // �������� ���������
    std::string value;
};

// �����, ������������ �������� �������� ���������.
class ExprConstContext : public ExprContext {
public:
    // �������� ���������� ��������
    ExprConstContext(ConstContext* constant) {
        this->constant = constant;
    }

    // ����� ���������� ���������� � ��������� ��������
    virtual void debugOut() {
        std::cout << "CONST";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "CONST";
    }
    virtual std::string getResType() {
        return constant->getValue()->getType();
    }
    virtual void generateAsmCode();
private:
    // �������� ���������
    ConstContext* constant;
};

// �����, ������������ �������� �������� ����������.
class ExprVarContext : public ExprContext {
public:
    // �������� ���������� ��������
    ExprVarContext(VarContext* variable) {
        this->variable = variable;
    }

    // ����� ���������� ���������� � ��������� ��������
    virtual void debugOut() {
        std::cout << "VAR " + variable->getName();
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "VAR";
    }
    virtual std::string getResType() {
        return variable->getType()->getTypeName();
    }
    virtual void generateAsmCode();

    virtual Register* getAssignedReg() {
        if (assignedRegister == nullptr) {
            assignedRegister = variable->getAssignedReg();
        }
        return assignedRegister;
    }

    VarContext* getVariable() {
        return variable;
    }
private:
    // ����������
    VarContext* variable;
};

// �����, ������������ �������� NIL ��������.
class ExprNilContext : public ExprContext {
public:
    // �������� NIL ��������
    ExprNilContext() { }

    // ����� ���������� ���������� � NIL ��������
    virtual void debugOut() {
        std::cout << "NIL";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "NIL";
    }
    virtual void generateAsmCode();
};

// �����, ������������ �������� ������.
class ExprErrContext : public ExprContext {
public:
    // �������� ������
    ExprErrContext(std::string errMsg) : errMsg(errMsg) { }

    // ����� ���������� ���������� � ��������� ������
    virtual void debugOut() {
        std::cout << "ERROR: "<<errMsg;
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "ERR";
    }
    virtual std::string getResType() {
        return "ERR: "+errMsg;
    }
    virtual void generateAsmCode() {}
    std::string getErrMsg() {
        return errMsg;
    }
private:
    // ��������� �� ������
    std::string errMsg;
};

// �����, ������������ �������� ���������.
class ExprExprContext : public ExprContext {
public:
    // �������� ���������
    ExprExprContext(ExprContext* leftOp, ExprContext* rightOp, std::string operName) {
        left = leftOp;
        right = rightOp;
        this->operName = operName;
    }
    // ����� ���������� ���������� � ���������
    virtual void debugOut() {
        std::cout << "( ";
        left->debugOut();
        std::cout << " " << operName << " ";
        right->debugOut();
        std::cout << " )";
    }
    virtual std::string getResType();
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "EXPR";
    }
    ExprContext* getLeft() { return left; }
    ExprContext* getRight() { return right; }
    std::string getOperator() { return operName; }
    virtual void generateAsmCode();
private:
    // ����� �������
    ExprContext* left;
    // ������ �������
    ExprContext* right;
    // �������� ���������
    std::string operName;
};

// �����, ������������ �������� �������� ������������ ���������.
class ExprUnarExprContext : public ExprContext {
public:
    // �������� ������������ ���������
    ExprUnarExprContext(ExprContext* operand, std::string operName) {
        this->operand = operand;
        this->operName = operName;
    }
    // ����� ���������� ���������� � ����������� ���������
    virtual void debugOut() {
        std::cout << "( " << operName << " ";
        operand->debugOut();
        std::cout <<  " )";
    }
    virtual std::string getResType();
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "EXPR";
    }

    virtual void generateAsmCode();
private:
    // �������
    ExprContext* operand;
    // �������� ���������
    std::string operName;
};

class ProcContext;

// �����, ������������ �������� �������� ������������ ���������.
class ExprProcCallContext : public ExprContext {
public:
    // �������� ������������ ���������
    ExprProcCallContext(ProcContext* proc) {
        this->proc = proc;
    }
    void addActualParam(ExprContext* param) { actualParams.push_back(param); }
    // ����� ���������� ���������� � ����������� ���������
    virtual void debugOut();
    virtual std::string getResType();
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "EXPR";
    }

    bool checkParams();
    virtual void generateAsmCode();
private:
    // ���������
    ProcContext* proc;
    // ������ ����������
    std::vector<ExprContext*> actualParams;
};

#endif // EXPR_H