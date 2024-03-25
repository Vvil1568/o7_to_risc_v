#ifndef EXPR_H
#define EXPR_H

#include "context.h"
#include "variable.h"
#include "const.h"

// �����, ������������ �������� ���������.
// ��������������, ��� � ����� ���������� �������� �������� �����
class ExprContext : public Context {
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
        return "STRING";
    }
    std::string getStrValue() {
        return value;
    }
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
        std::cout << "VAR";
    }
    // ���������� � ���� ���������
    virtual std::string getType() {
        return "VAR";
    }
    virtual std::string getResType() {
        return variable->getType()->getTypeName();
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
private:
    // �������
    ExprContext* operand;
    // �������� ���������
    std::string operName;
};

#endif // EXPR_H