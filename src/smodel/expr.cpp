#include "expr.h"

std::string ExprExprContext::getResType() {
    if (left->getResType().substr(0,3) == "ERR") {
        return left->getResType();
    }
    if (right->getResType().substr(0, 3) == "ERR") {
        return right->getResType();
    }
    if (operName == "OR" || operName == "&") {
        if (left->getResType() != "BOOL" || right->getResType() != "BOOL") {
            return "ERR: "+("Unsupported operation " + operName + " between types " + left->getResType() + " and " + right->getResType());
        }
        return "BOOL";
    }
    if (operName == "DIV" || operName == "MOD") {
        if (left->getResType() != "INT" || right->getResType() != "INT") {
            return "ERR: " + ("Unsupported operation " + operName + " between types " + left->getResType() + " and " + right->getResType());
        }
        return "INT";
    }
    if (operName == "+" || operName == "-" || operName == "*" || operName == "/") {
        if ((left->getResType() != right->getResType())) {
            return "ERR: "+("Unsupported operation " + operName + " between different types " + left->getResType() + " and " + right->getResType());
        }
        if ((left->getResType() == "REAL")) {
            return "REAL";
        }
        else if ((left->getResType() == "INT")) {
            return "INT";
        }
        else {
            return "ERR: "+("Unsupported operation " + operName + " between " + left->getResType() + "s");
        }
    }
    if (operName == "=" || operName == "#") {
        if ((left->getResType() != right->getResType())) {
            return "ERR: "+("Unsupported operation " + operName + " between different types " + left->getResType() + " and " + right->getResType());
        }
        if (left->getResType() == "REAL" || left->getResType() == "INT" || left->getResType() == "BOOL" || left->getResType() == "STRING") {
            return "BOOL";
        }
        return "ERR: "+("Unsupported operation " + operName + " between " + left->getType() + "s");
    }
    if (operName == ">" || operName == "<" || operName == "<=" || operName == ">=") {
        if ((left->getResType() != right->getResType())) {
            return "ERR:"+("Unsupported operation " + operName + " between different types " + left->getResType() + " and " + right->getResType());
        }
        if ((left->getResType() == "REAL") || (left->getResType() == "INT")) {
            return "BOOL";
        }
        return "ERR:" + ("Unsupported operation " + operName + " between " + left->getType() + "s");
    }
    return "ERR:" + ("Unsupported operator: " + operName);
}


std::string ExprUnarExprContext::getResType() {
    if (operand->getResType().substr(0, 3) == "ERR") {
        return operand->getResType();
    }
    if (operName == "~") {
        if (operand->getResType() != "BOOL") {
            return "ERR: " + ("Unsupported operation " + operName + " for type " + operand->getResType());
        }
        return "BOOL";
    }
    if (operName == "+" || operName == "-") {
        if (operand->getResType() != "INT" && operand->getResType() != "REAL") {
            return "ERR: " + ("Unsupported operation " + operName + " for type " + operand->getResType());
        }
        return operand->getResType();
    }
    return "ERR:" + ("Unsupported operator: " + operName);
}

void ExprIntContext::generateAsmCode() {
    CodeGenContext::addCodeLine("#Загружаем целочисленный литерал " + std::to_string(value) 
        + " в регистр " + getAssignedReg()->getName());
    CodeGenContext::addCodeLine("li "+getAssignedReg()->getName() + " " + std::to_string(value));
}

void ExprRealContext::generateAsmCode() {
    std::string dataLabel = std::to_string(value);
    std::replace(dataLabel.begin(), dataLabel.end(), '.', '_');
    dataLabel = "literal_" + dataLabel;
    CodeGenContext::addDataLine("#Вещественный литерал " + std::to_string(value));
    CodeGenContext::addDataLine(dataLabel + ": .double " + std::to_string(value));
    Register* temp = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
    CodeGenContext::addCodeLine("#Загружаем вещественный литерал по метке "+ dataLabel + 
        " в регистр " + getAssignedReg()->getName() + " через промежуточный регистр " + temp->getName());
    CodeGenContext::addCodeLine("fld " + getAssignedReg()->getName() + " " + dataLabel + " " + temp->getName());
    RegisterPool::getInstance().freeRegister(temp);
}

void ExprBooleanContext::generateAsmCode() {
    CodeGenContext::addCodeLine("#Загружаем булевый литерал как число " + std::to_string(value)
        + " в регистр " + getAssignedReg()->getName());
    CodeGenContext::addCodeLine("li " + getAssignedReg()->getName() + " " + (value?"1":"0"));
}

void ExprStringContext::generateAsmCode() {
    std::string dataLabel = CodeGenContext::getStrLitLabel();

    CodeGenContext::addDataLine("#Строковый литерал " + value);
    CodeGenContext::addDataLine(dataLabel + ": .asciz " + value);
    CodeGenContext::addCodeLine("#Загружаем адрес строкового литерала по метке " + dataLabel +
        " в регистр " + getAssignedReg()->getName());
    CodeGenContext::addCodeLine("la " + getAssignedReg()->getName() + " " + dataLabel);
}

void ExprNilContext::generateAsmCode() {
    CodeGenContext::addCodeLine("#Загружаем NIL (нулевой адрес) как число в регистр " + getAssignedReg()->getName());
    CodeGenContext::addCodeLine("li " + getAssignedReg()->getName() + " 0");
}

void ExprVarContext::generateAsmCode() {
    CodeGenContext::addCodeLine("#Копируем содержимое переменной "+variable->getName() + 
        ", который был назначен регистр " + variable->getAssignedReg()->getName() + "  в регистр " + getAssignedReg()->getName());
    CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " " + variable->getAssignedReg()->getName());
}

void ExprConstContext::generateAsmCode() {
    if (constant->getType() == "EXPR") {
        constant = constant->getValue();
    }
    if (constant->getType() == "ERR") {
        return;
    }
    ExprContext* typedContext = nullptr;
    if (constant->getType() == "INT") {
        int val = (static_cast<ConstIntContext*>(constant))->getIntValue();
        typedContext = new ExprIntContext(val);
    }
    if (constant->getType() == "REAL") {
        double val = (static_cast<ConstRealContext*>(constant))->getRealValue();
        typedContext = new ExprRealContext(val);
    }
    if (constant->getType() == "BOOL") {
        bool val = (static_cast<ConstBooleanContext*>(constant))->getBoolValue();
        typedContext = new ExprBooleanContext(val);
    }
    if (constant->getType() == "STRING") {
        std::string val = (static_cast<ConstStringContext*>(constant))->getStrValue();
        typedContext = new ExprStringContext(val);
    }
    if (constant->getType() == "NIL") {
        typedContext = new ExprNilContext();
    }
    if (typedContext != nullptr) {
        typedContext->AssignReg(getAssignedReg());
        typedContext->generateAsmCode();
    }
}

void ExprUnarExprContext::generateAsmCode() {
    operand->generateAsmCode();
    if (assignedRegister == nullptr) {
        if (operand->getType() == "VAR") {
            if (operand->getResType() == "REAL") {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
            }
            else {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
            }
        }
        else {
            assignedRegister = operand->getAssignedReg();
        }
    }
    if (operName == "~") {
        CodeGenContext::addCodeLine("#Отрицаем содержимое регистра " + operand->getAssignedReg()->getName() +
            " и кладем результат в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("not " + getAssignedReg()->getName() + " " + operand->getAssignedReg()->getName());
    }
    if (operName == "-" || operName == "+") {
        if (operName == "-") {
            CodeGenContext::addCodeLine("#Приписываем минус к содержимому регистра " + operand->getAssignedReg()->getName() +
                " и кладем результат в регистр " + getAssignedReg()->getName());
            CodeGenContext::addCodeLine("neg " + getAssignedReg()->getName() + " " + operand->getAssignedReg()->getName());
        }
        if (operName == "+") {
            CodeGenContext::addCodeLine("Ничего не делаем с содержимым регистра " + operand->getAssignedReg()->getName() +
                " и кладем результат в регистр " + getAssignedReg()->getName());
            CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " " + operand->getAssignedReg()->getName());
        }
    }
}

void ExprExprContext::generateAsmCode() {
    left->generateAsmCode();
    right->generateAsmCode();
    if (assignedRegister == nullptr) {
        if (left->getType() == "VAR" && right->getType() == "VAR") {
            if (left->getResType() == "REAL" && (operName == "+" || operName == "-" || operName == "*" || operName == "/")) {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
            }
            else {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
            }
        }
        else {
            if (left->getResType() == "REAL" && !(operName == "+" || operName == "-" || operName == "*" || operName == "/")) {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
            }
            else {
                assignedRegister = left->getAssignedReg();
            }
        }
    }

    if (operName == "OR") {
        CodeGenContext::addCodeLine("#Производим операцию ИЛИ для содержимого регистров " + left->getAssignedReg()->getName() + 
            " и "+ right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("or " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "&") {
        CodeGenContext::addCodeLine("#Производим операцию И для содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("and " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "DIV") {
        CodeGenContext::addCodeLine("#Ищем целую часть от деления содержимого регистра " + left->getAssignedReg()->getName() +
            " на содержимое регистра " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("div " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "MOD") {
        CodeGenContext::addCodeLine("#Ищем остаток от деления содержимого регистра " + left->getAssignedReg()->getName() +
            " на содержимое регистра " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("rem " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "+") {
        CodeGenContext::addCodeLine("#Ищем сумму содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fadd.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("add " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "-") {
        CodeGenContext::addCodeLine("#Ищем разность содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fsub.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("sub " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "*") {
        CodeGenContext::addCodeLine("#Ищем произведение содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fmul.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("mul " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "/") {
        CodeGenContext::addCodeLine("#Ищем сумму содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fdiv.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("div " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    /*
    if (operName == "=" || operName == "#") {
        if ((left->getResType() != right->getResType())) {
            return "ERR: " + ("Unsupported operation " + operName + " between different types " + left->getResType() + " and " + right->getResType());
        }
        if (left->getResType() == "REAL" || left->getResType() == "INT" || left->getResType() == "BOOL" || left->getResType() == "STRING") {
            return "BOOL";
        }
        return "ERR: " + ("Unsupported operation " + operName + " between " + left->getType() + "s");
    }
    if (operName == ">" || operName == "<" || operName == "<=" || operName == ">=") {
        if ((left->getResType() != right->getResType())) {
            return "ERR:" + ("Unsupported operation " + operName + " between different types " + left->getResType() + " and " + right->getResType());
        }
        if ((left->getResType() == "REAL") || (left->getResType() == "INT")) {
            return "BOOL";
        }
        return "ERR:" + ("Unsupported operation " + operName + " between " + left->getType() + "s");
    }
    */

    if (left->getType() != "VAR" && left->getAssignedReg() != assignedRegister) {
        RegisterPool::getInstance().freeRegister(left->getAssignedReg());
    }
    if (right->getType() != "VAR" && right->getAssignedReg() != assignedRegister) {
        RegisterPool::getInstance().freeRegister(right->getAssignedReg());
    }
}