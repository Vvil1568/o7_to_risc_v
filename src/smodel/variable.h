#ifndef VARIABLE_H
#define VARIABLE_H

#include "type.h"
#include "registerpool.h"
#include "codegencontext.h"
#include "designator.h"

// Класс, задающий контекст для всех переменых
class VarContext: public Context {
public:
    // Конструктор для создания переменной заданного типа
    VarContext(std::string name, TypeContext* type = nullptr);

    // Получение информации о типе переменой
    TypeContext* getType();
    // Установка типа переменной
    void setType(TypeContext* type) { context = type; }

    std::string getName() { return varName; }

    // Вывод отладочной информации о базовом типе
    virtual void debugOut();

    void setOnStack() { this->onStack = true; }
    void setOnData(std::string dataBegLabel) { 
        this->onData = true; 
        this->dataBegLabel = dataBegLabel;
    }

    bool isInMemory() { return (ref!=nullptr && (ref->onStack || ref->onData)) || (onStack || onData); }

    int getMemoryOffset() { 
        if (ref != nullptr) return ref->memOffset;
        return memOffset; 
    }

    std::string getDataBegLabel() {
        if (ref != nullptr) return ref->dataBegLabel;
        return dataBegLabel;
    }

    void setMemoryOffset(int offset) { memOffset = offset; }

    void loadFromMemory();
    void saveToMemory();

    virtual void AssignReg(Register* reg) { assignedRegister = reg; }
    virtual void ClearReg() { 
        assignedRegister = nullptr; 
    }
    virtual Register* getAssignedReg() {
        if (isInMemory()) {
            if (assignedRegister == nullptr) {
                if (context->getTypeName() == "REAL") {
                    assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
                }
                else {
                    assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
                }
            }
            return assignedRegister;
        }
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
    void addDesignatorAction(DesignatorActionContext* action) { designatorActions.push_back(action); }

    VarContext* Copy() {
        VarContext* newVar = new VarContext(varName, context);
        newVar->ref = this;
        return newVar;
    }

    TypeContext* performDesignatorActions(Register* addrReg);
protected:
    Register* assignedRegister = nullptr;
    TypeContext* context;   // Тип переменной
    std::string varName; //Название переменной
    std::vector<DesignatorActionContext*> designatorActions;
    bool onStack = false;
    bool onData = false;
    std::string dataBegLabel;
    int memOffset = 0;
    VarContext* ref = nullptr;
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
        if (isInMemory()) return assignedRegister;
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
