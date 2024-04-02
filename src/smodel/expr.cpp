#include "expr.h"
#include "procedure.h"

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
        if (left->getResType() == "REAL" || left->getResType() == "INT" || left->getResType() == "BOOL" || left->getResType() == "ARRAY OF CHAR") {
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
    if (variable->isOnStack()) {
        if (variable->getType()->getTypeName() == "REAL") {
            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
        }
        else {
            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
        }
        int addrOffset = variable->getStackOffset();
        CodeGenContext::addCodeLine("#Получаем значение переменной " + variable->getName() +
            " со стека, по адресу sp" + std::to_string(addrOffset) + "  в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("lw " + getAssignedReg()->getName() + " "+ std::to_string(addrOffset)+"(sp)");
    }else if (getAssignedReg() != variable->getAssignedReg()) {
        CodeGenContext::addCodeLine("#Копируем содержимое переменной " + variable->getName() +
            ", которой был назначен регистр " + variable->getAssignedReg()->getName() + ",  в регистр " + getAssignedReg()->getName());
        CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " " + variable->getAssignedReg()->getName());
    }
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
    if (constant->getType() == "ARRAY OF CHAR") {
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
        if ((left->getType() == "VAR" && !static_cast<ExprVarContext*>(left)->getVariable()->isOnStack()) &&
            right->getType() == "VAR" && !static_cast<ExprVarContext*>(right)->getVariable()->isOnStack()) {
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
                if (left->getType() != "VAR" || static_cast<ExprVarContext*>(left)->getVariable()->isOnStack()) {
                    assignedRegister = left->getAssignedReg();
                }
                else {
                    assignedRegister = right->getAssignedReg();
                }
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
    if (operName == "=") {
        CodeGenContext::addCodeLine("#Проверяем равенство содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("feq.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("#Для этого найдем их разность");
            CodeGenContext::addCodeLine("sub " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("#И проверим ее на равенство нулю");
            CodeGenContext::addCodeLine("seqz " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }
    if (operName == "#") {
        CodeGenContext::addCodeLine("#Проверяем неравенство содержимого регистров " + left->getAssignedReg()->getName() +
            " и " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("#Для этого проверим их на равенство");
            CodeGenContext::addCodeLine("feq.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("#А потом инвертируем результат");
            CodeGenContext::addCodeLine("not " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("#Для этого найдем их разность");
            CodeGenContext::addCodeLine("sub " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("#И проверим ее неравенство нулю");
            CodeGenContext::addCodeLine("snez " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }
    if (operName == "<") {
        CodeGenContext::addCodeLine("#Проверяем, что содержимое регистра " + left->getAssignedReg()->getName() +
            " меньше содержимого регистра " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("flt.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("slt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == ">") {
        CodeGenContext::addCodeLine("#Проверяем, что содержимое регистра " + left->getAssignedReg()->getName() +
            " больше содержимого регистра " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fgt.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("sgt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "<=") {
        CodeGenContext::addCodeLine("#Проверяем, что содержимое регистра " + left->getAssignedReg()->getName() +
            " меньше или равно содержимого регистра " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fle.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("#Для этого проверяем, что содержимое регистра " + left->getAssignedReg()->getName() +
                " больше содержимого регистра " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("sgt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("#А потом инвертируем результат");
            CodeGenContext::addCodeLine("not " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }

    if (operName == ">=") {
        CodeGenContext::addCodeLine("#Проверяем, что содержимое регистра " + left->getAssignedReg()->getName() +
            " больше или равно содержимого регистра " + right->getAssignedReg()->getName() + " и кладем результат в регистр " + getAssignedReg()->getName());
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fge.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("#Для этого проверяем, что содержимое регистра " + left->getAssignedReg()->getName() +
                " меньше содержимого регистра " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("slt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeLine("#А потом инвертируем результат");
            CodeGenContext::addCodeLine("not " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }

    if ((left->getType() != "VAR" || static_cast<ExprVarContext*>(left)->getVariable()->isOnStack()) && left->getAssignedReg() != assignedRegister) {
        RegisterPool::getInstance().freeRegister(left->getAssignedReg());
    }
    if ((right->getType() != "VAR" || static_cast<ExprVarContext*>(right)->getVariable()->isOnStack()) && right->getAssignedReg() != assignedRegister) {
        RegisterPool::getInstance().freeRegister(right->getAssignedReg());
    }
}

std::string ExprProcCallContext::getResType() {
    if (proc->getResultType() == nullptr) return "";
    return proc->getResultType()->getTypeName();
}

bool ExprProcCallContext::checkParams() { //TODO - подключить
    std::vector<ArgVarContext*> formalParams = proc->GetFormalParams();
    if (formalParams.size() != actualParams.size()) return false;
    for (int i = 0; i < formalParams.size();i++) {
        if (formalParams[i]->getType()->getTypeName() != actualParams[i]->getResType()) {
            return false;
        }
    }
    return true;
}

void ExprProcCallContext::generateAsmCode() {
    if (proc->getStackFrameSize() != 0) {
        CodeGenContext::addCodeLine("#Выделяем на стеке память для данных процедуры");
        CodeGenContext::addCodeLine("addi sp sp " + std::to_string(proc->getStackFrameSize()));
    }

    std::vector<ArgVarContext*> formalParams = proc->GetFormalParams();
    for (int i = 0; i < formalParams.size(); i++) {
        CodeGenContext::addCodeLine("#Загружаем "+std::to_string(i) + "-й аргумент");
        (new AssignmentStatementContext(formalParams[i], actualParams[i]))->generateAsmCode(); //TODO в ссылочный аргумент - только переменная 
    }
    CodeGenContext::addCodeLine("#Заходим в процедуру");
    CodeGenContext::addCodeLine("jal " + proc->getLabel());

    for (int i = 0; i < formalParams.size(); i++) {
        if (formalParams[i]->isVar()) {
            CodeGenContext::addCodeLine("#Параметр \"" + formalParams[i]->getName() + " является ссылочным, поэтому");
            CodeGenContext::addCodeLine("#Переносим последнее значение регистра параметра в регистр передаваемого аргумента");
            (new AssignmentStatementContext(dynamic_cast<ExprVarContext*>(actualParams[i])->getVariable(), new ExprVarContext(formalParams[i])))->generateAsmCode();
        }
    }

    if (proc->getResultType() != nullptr) {
        CodeGenContext::addCodeLine("#Переносим значение, возвращенное процедурой в регистр "+getAssignedReg()->getName());
        if (proc->getResultType()->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("fmv.d " + getAssignedReg()->getName() + " fa0");
        }
        else {
            CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " a0");
        }
    }

    if (proc->getStackFrameSize() != 0) {
        CodeGenContext::addCodeLine("#Высвобождаем на стеке память, выделенную для данных функции");
        CodeGenContext::addCodeLine("addi sp sp " + std::to_string(-proc->getStackFrameSize()));
    }
}

void ExprProcCallContext::debugOut() {
    std::cout << proc->getProcedureName() + "(...)";
    std::cout << " )";
}