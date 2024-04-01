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

// Класс, определяющий контекст выражения.
// Предполагается, что в языке существуют литералы основных типов
class ExprContext : public Context {
protected:
    Register* assignedRegister = nullptr;
public:
    // Вывод отладочной информации о контексте импорта
    virtual void debugOut() {

    }

    virtual std::string getResType() {
        return getType();
    }
    // Информации о типе контекста
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

// Класс, определяющий контекст целочисленного литерала.
class ExprIntContext : public ExprContext {
public:
    // Создание целочисленного литерала
    ExprIntContext(int v) : value(v) { }

    // Вывод отладочной информации о целочисленном литерале
    virtual void debugOut() {
        std::cout << value;
    }
    // Информации о типе контекста
    virtual std::string getType() {
        return "INT";
    }
    int getIntValue() {
        return value;
    }
    virtual void generateAsmCode();
private:
    // Значение литерала
    int value;
};

// Класс, определяющий контекст вещественного литерала.
class ExprRealContext : public ExprContext {
public:
    // Создание вещественного литерала
    ExprRealContext(double v) : value(v) { }

    // Вывод отладочной информации о вещественном литерале
    virtual void debugOut() {
        std::cout << value;
    }
    // Информации о типе контекста
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
    // Значение литерала
    double value;
};

// Класс, определяющий контекст булевого литерала.
class ExprBooleanContext : public ExprContext {
public:
    // Создание булевого литерала
    ExprBooleanContext(bool v) : value(v) { }

    // Вывод отладочной информации о булевом литерале
    virtual void debugOut() {
        std::cout << value;
    }
    // Информации о типе контекста
    virtual std::string getType() {
        return "BOOL";
    }
    bool getBoolValue() {
        return value;
    }
    virtual void generateAsmCode();
private:
    // Значение константы
    bool value;
};

// Класс, определяющий контекст строкового литерала.
class ExprStringContext : public ExprContext {
public:
    // Создание строкового литерала
    ExprStringContext(std::string v) : value(v) { }

    // Вывод отладочной информации о строковом литерале
    virtual void debugOut() {
        std::cout << value;
    }
    // Информации о типе контекста
    virtual std::string getType() {
        return "ARRAY OF CHAR";
    }
    std::string getStrValue() {
        return value;
    }
    virtual void generateAsmCode();
private:
    // Значение константы
    std::string value;
};

// Класс, определяющий контекст значения константы.
class ExprConstContext : public ExprContext {
public:
    // Создание строкового литерала
    ExprConstContext(ConstContext* constant) {
        this->constant = constant;
    }

    // Вывод отладочной информации о строковом литерале
    virtual void debugOut() {
        std::cout << "CONST";
    }
    // Информации о типе контекста
    virtual std::string getType() {
        return "CONST";
    }
    virtual std::string getResType() {
        return constant->getValue()->getType();
    }
    virtual void generateAsmCode();
private:
    // Значение константы
    ConstContext* constant;
};

// Класс, определяющий контекст значения переменной.
class ExprVarContext : public ExprContext {
public:
    // Создание строкового литерала
    ExprVarContext(VarContext* variable) {
        this->variable = variable;
    }

    // Вывод отладочной информации о строковом литерале
    virtual void debugOut() {
        std::cout << "VAR " + variable->getName();
    }
    // Информации о типе контекста
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
    // Переменная
    VarContext* variable;
};

// Класс, определяющий контекст NIL литерала.
class ExprNilContext : public ExprContext {
public:
    // Создание NIL литерала
    ExprNilContext() { }

    // Вывод отладочной информации о NIL литерале
    virtual void debugOut() {
        std::cout << "NIL";
    }
    // Информации о типе контекста
    virtual std::string getType() {
        return "NIL";
    }
    virtual void generateAsmCode();
};

// Класс, определяющий контекст ошибки.
class ExprErrContext : public ExprContext {
public:
    // Создание ошибки
    ExprErrContext(std::string errMsg) : errMsg(errMsg) { }

    // Вывод отладочной информации о константе ошибки
    virtual void debugOut() {
        std::cout << "ERROR: "<<errMsg;
    }
    // Информации о типе контекста
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
    // Сообщение об ошибке
    std::string errMsg;
};

// Класс, определяющий контекст выражения.
class ExprExprContext : public ExprContext {
public:
    // Создание выражения
    ExprExprContext(ExprContext* leftOp, ExprContext* rightOp, std::string operName) {
        left = leftOp;
        right = rightOp;
        this->operName = operName;
    }
    // Вывод отладочной информации о выражении
    virtual void debugOut() {
        std::cout << "( ";
        left->debugOut();
        std::cout << " " << operName << " ";
        right->debugOut();
        std::cout << " )";
    }
    virtual std::string getResType();
    // Информации о типе контекста
    virtual std::string getType() {
        return "EXPR";
    }
    ExprContext* getLeft() { return left; }
    ExprContext* getRight() { return right; }
    std::string getOperator() { return operName; }
    virtual void generateAsmCode();
private:
    // Левый операнд
    ExprContext* left;
    // Правый операнд
    ExprContext* right;
    // Название оператора
    std::string operName;
};

// Класс, определяющий контекст унарного константного выражения.
class ExprUnarExprContext : public ExprContext {
public:
    // Создание константного выражения
    ExprUnarExprContext(ExprContext* operand, std::string operName) {
        this->operand = operand;
        this->operName = operName;
    }
    // Вывод отладочной информации о константном выражении
    virtual void debugOut() {
        std::cout << "( " << operName << " ";
        operand->debugOut();
        std::cout <<  " )";
    }
    virtual std::string getResType();
    // Информации о типе контекста
    virtual std::string getType() {
        return "EXPR";
    }

    virtual void generateAsmCode();
private:
    // Операнд
    ExprContext* operand;
    // Название оператора
    std::string operName;
};

class ProcContext;

// Класс, определяющий контекст унарного константного выражения.
class ExprProcCallContext : public ExprContext {
public:
    // Создание константного выражения
    ExprProcCallContext(ProcContext* proc) {
        this->proc = proc;
    }
    void addActualParam(ExprContext* param) { actualParams.push_back(param); }
    // Вывод отладочной информации о константном выражении
    virtual void debugOut();
    virtual std::string getResType();
    // Информации о типе контекста
    virtual std::string getType() {
        return "EXPR";
    }

    bool checkParams();
    virtual void generateAsmCode();
private:
    // Процедура
    ProcContext* proc;
    // Список аргументов
    std::vector<ExprContext*> actualParams;
};

#endif // EXPR_H