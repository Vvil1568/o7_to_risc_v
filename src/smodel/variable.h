#ifndef VARIABLE_H
#define VARIABLE_H

#include "type.h"
#include "registerpool.h"

// Класс, задающий контекст для всех переменых
class VarContext: public Context {
public:
    // Конструктор для создания переменной заданного типа
    VarContext(std::string name, TypeContext* type = nullptr);

    // Получение информации о типе переменой
    TypeContext* getType() { return context; }
    // Установка типа переменной
    void setType(TypeContext* type) { context = type; }

    std::string getName() { return varName; }

    // Вывод отладочной информации о базовом типе
    virtual void debugOut();

    void setOnStack() { this->onStack = true; }

    bool isOnStack() { return onStack; }

    int getStackOffset() { return stackOffset; }

    void setStackOffset(int offset) { stackOffset = offset; }

    virtual Register* getAssignedReg() {
        if (onStack) return nullptr;
        if (assignedRegister == nullptr) {
            if (context->getTypeName() == "REAL") {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FS);
            }
            else {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::S);
            }  
        }
        if (assignedRegister == nullptr) {
            setOnStack();
        }
        return assignedRegister;
    }

protected:
    Register* assignedRegister = nullptr;
    TypeContext* context;   // Тип переменной
    std::string varName; //Название переменной
    bool onStack = false;
    int stackOffset = 0;
};

class ArgVarContext : public VarContext {
public:
    ArgVarContext(std::string name, TypeContext* type, bool var = false):VarContext(name, type) {
        this->var = var;
    }

    bool isVar() {
        return var;
    }

    virtual Register* getAssignedReg() {
        if (assignedRegister == nullptr) {
            if (context->getTypeName() == "REAL") {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FA);
            }
            else {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::A);
            }
        }
        if (assignedRegister == nullptr) {
            setOnStack();
        }
        return assignedRegister;
    }
private:
    bool var;
}; 

#endif // VARIABLE_H
