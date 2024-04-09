#include "variable.h"

// Конструктор для создания переменной заданного типа
VarContext::VarContext(std::string name, TypeContext* type): context(type) {
    varName = name;
}

void VarContext::loadFromMemory() {
    if ((ref!=nullptr && ref->onStack) || onStack) {
        if (designatorActions.size() == 0) {
            CodeGenContext::addCodeComment("Loading the " + getName() +
                " variable value from the stack from the address sp+" + std::to_string(getMemoryOffset()) + " into " + getAssignedReg()->getName() +" register");
            if (context->getTypeName() == "REAL") {
                CodeGenContext::addCodeLine("fld " + getAssignedReg()->getName() + " " + std::to_string(getMemoryOffset()) + "(sp)");
            }
            else {
                CodeGenContext::addCodeLine("lw " + getAssignedReg()->getName() + " " + std::to_string(getMemoryOffset()) + "(sp)");
            }
        }
        else {
            Register* adrReg = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
            CodeGenContext::addCodeComment("Loading the stack pointer into " + adrReg->getName() + " register");
            CodeGenContext::addCodeLine("mv " + adrReg->getName() + " sp");
            CodeGenContext::addCodeComment("Shifting the address to the beginning of the " + getName()+" variable data");
            CodeGenContext::addCodeLine("addi " + adrReg->getName() + " " + adrReg->getName() + " " + std::to_string(getMemoryOffset()));
            TypeContext* type = performDesignatorActions(adrReg);
            CodeGenContext::addCodeComment("Loading the " + getName() + " variable value from the stack using the resulting address into " + getAssignedReg()->getName()+" register");
            if (type->getTypeName() == "REAL") {
                CodeGenContext::addCodeLine("fld " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
            }
            else {
                CodeGenContext::addCodeLine("lw " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
            }
            RegisterPool::getInstance().freeRegister(adrReg);
        }
    }
    if ((ref != nullptr && ref->onData) || onData) {
        Register* adrReg = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
        CodeGenContext::addCodeComment("Loading the " + getDataBegLabel() + " label address into " + adrReg->getName()+" register");
        CodeGenContext::addCodeLine("la " + adrReg->getName() + " " + getDataBegLabel());
        TypeContext* type = performDesignatorActions(adrReg);
        CodeGenContext::addCodeComment("Loading the " + getName() + " variable value from the data segment using the resulting address into " + getAssignedReg()->getName()+" register");
        if (type->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("fld " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
        }
        else {
            CodeGenContext::addCodeLine("lw " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
        }
        RegisterPool::getInstance().freeRegister(adrReg);
    }
}

void VarContext::saveToMemory() {
    if ((ref != nullptr && ref->onStack) || onStack) {
        if (designatorActions.size() == 0) {
            CodeGenContext::addCodeComment("Saving the value to the stack to the address sp+" + std::to_string(getMemoryOffset()));
            if (context->getTypeName() == "REAL") {
                CodeGenContext::addCodeLine("fsd " + getAssignedReg()->getName() + " " + std::to_string(getMemoryOffset()) + "(sp)");
            }
            else {
                CodeGenContext::addCodeLine("sw " + getAssignedReg()->getName() + " " + std::to_string(getMemoryOffset()) + "(sp)");
            }
        }
        else {
            Register* adrReg = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
            CodeGenContext::addCodeComment("Loading the stack pointer into " + adrReg->getName()+" register");
            CodeGenContext::addCodeLine("mv " + adrReg->getName() + " sp");
            CodeGenContext::addCodeComment("Shifting the address to the beginning of the " + getName()+" variable data");
            CodeGenContext::addCodeLine("addi " + adrReg->getName() + " " + adrReg->getName() + " " + std::to_string(getMemoryOffset()));

            TypeContext* type = performDesignatorActions(adrReg);
            CodeGenContext::addCodeComment("Saving the " + getName() + " variable value to the stack using the resulting address from the " + getAssignedReg()->getName() + " register");
            if (type->getTypeName() == "REAL") {
                CodeGenContext::addCodeLine("fsd " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
            }
            else {
                CodeGenContext::addCodeLine("sw " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
            }
            RegisterPool::getInstance().freeRegister(adrReg);
        }
    }
    if ((ref != nullptr && ref->onData) || onData) {
        Register* adrReg = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
        CodeGenContext::addCodeComment("Loading the " + getDataBegLabel() + " label address into " + adrReg->getName()+" register");
        CodeGenContext::addCodeLine("la " + adrReg->getName() + " " + getDataBegLabel());

        TypeContext* type = performDesignatorActions(adrReg);
        CodeGenContext::addCodeComment("Saving the " + getName() + " variable value into the data segment, using the resulting address from the " + getAssignedReg()->getName()+" register");
        if (type->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("fsd " + getAssignedReg()->getName() + " (" + adrReg->getName()+")");
        }
        else {
            CodeGenContext::addCodeLine("sw " + getAssignedReg()->getName() + " (" + adrReg->getName() + ")");
        }
        RegisterPool::getInstance().freeRegister(adrReg);
    }
}

TypeContext* VarContext::getType() {
    TypeContext* curType = context;
    for (DesignatorActionContext* action : designatorActions) {
        if (curType->getTypeName().size() >= 5 && curType->getTypeName().substr(0, 5) == "ARRAY" && dynamic_cast<IndexDesignActionContext*>(action) != nullptr) {
            curType = static_cast<TypeArrayContext*>(curType)->getElemType();
        }
        else if (curType->getTypeName() == "RECORD" && dynamic_cast<FieldGetDesignActionContext*>(action) != nullptr) {
            FieldGetDesignActionContext* fieldAction = (static_cast<FieldGetDesignActionContext*>(action));
            curType = static_cast<TypeRecordContext*>(curType)->getFieldType(fieldAction->getName());
        }
        else if (curType->getTypeName() == "POINTER" && dynamic_cast<PointerDerefDesignActionContext*>(action) != nullptr) {
            curType = static_cast<TypePointerContext*>(curType)->getPointedType();
        }
    }
    return curType;
}

TypeContext* VarContext::performDesignatorActions(Register* addrReg) {
    TypeContext* curType = context;
    for (DesignatorActionContext* action : designatorActions) {
        action->AssignReg(addrReg);
        if (curType->getTypeName().size() >= 5 && curType->getTypeName().substr(0, 5) == "ARRAY") {
            (static_cast<IndexDesignActionContext*>(action))->setType(curType);
            action->generateAsmCode();
            curType = static_cast<TypeArrayContext*>(curType)->getElemType();
        }
        else if (curType->getTypeName() == "RECORD") {
            FieldGetDesignActionContext* fieldAction = (static_cast<FieldGetDesignActionContext*>(action));
            fieldAction->setType(curType);
            action->generateAsmCode();
            curType = static_cast<TypeRecordContext*>(curType)->getFieldType(fieldAction->getName());
        }
        else if (curType->getTypeName() == "POINTER") {
            action->generateAsmCode();
            curType = static_cast<TypePointerContext*>(curType)->getPointedType();
        }
    }
    return curType;
}

// Вывод общей отладочной информации о переменной
void VarContext::debugOut() {
    std::cout << "VAR ";
    // Добавление информации о типе переменной
    context->debugOut();
}

