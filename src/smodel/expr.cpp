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
    CodeGenContext::addCodeComment("Loading the integer literal " + std::to_string(value) 
        + " into the  " + getAssignedReg()->getName() +" register");
    CodeGenContext::addCodeLine("li "+getAssignedReg()->getName() + " " + std::to_string(value));
}

void ExprRealContext::generateAsmCode() {
    std::string dataLabel = std::to_string(value);
    std::replace(dataLabel.begin(), dataLabel.end(), '.', '_');
    dataLabel = "literal_" + dataLabel;
    if (CodeGenContext::adRealLitLabel(dataLabel)) {
        CodeGenContext::addDataComment("Floating-point literal " + std::to_string(value));
        CodeGenContext::addDataLine(dataLabel + ": .double " + std::to_string(value));
    }
    Register* temp = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
    CodeGenContext::addCodeComment("Loading the floating point literal "+ dataLabel + 
        " into the " + getAssignedReg()->getName() + " register using  " + temp->getName() + " as temporary");
    CodeGenContext::addCodeLine("fld " + getAssignedReg()->getName() + " " + dataLabel + " " + temp->getName());
    RegisterPool::getInstance().freeRegister(temp);
}

void ExprBooleanContext::generateAsmCode() {
    CodeGenContext::addCodeComment("Loading the boolean literal as an integer " + std::to_string(value)
        + " into the  " + getAssignedReg()->getName() + " register");
    CodeGenContext::addCodeLine("li " + getAssignedReg()->getName() + " " + (value?"1":"0"));
}

void ExprStringContext::generateAsmCode() {
    std::string dataLabel = CodeGenContext::getStrLitLabel();

    CodeGenContext::addDataComment("String literal " + value);
    CodeGenContext::addDataLine(dataLabel + ": .asciz " + value);
    CodeGenContext::addCodeComment("Loading the string literal address using the label " + dataLabel +
        " into the literal " + getAssignedReg()->getName());
    CodeGenContext::addCodeLine("la " + getAssignedReg()->getName() + " " + dataLabel);
}

void ExprNilContext::generateAsmCode() {
    CodeGenContext::addCodeComment("Loading NIL (null pointer) as an integer into the " + getAssignedReg()->getName() +" register");
    CodeGenContext::addCodeLine("li " + getAssignedReg()->getName() + " 0");
}

void ExprVarContext::generateAsmCode() {
    if (variable->isInMemory()) {
        variable->AssignReg(getAssignedReg());
        variable->loadFromMemory();
    }else if (getAssignedReg() != variable->getAssignedReg()) {
        CodeGenContext::addCodeComment("Copying the " + variable->getName() +
            "variable contents, which has " + variable->getAssignedReg()->getName() + " register assigned to it, into the " + getAssignedReg()->getName() + " register");
        if (variable->getType()->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("fmv.d " + getAssignedReg()->getName() + " " + variable->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " " + variable->getAssignedReg()->getName());
        }
       
    }
}

void ExprConstContext::generateAsmCode() {
    if (constant->getType() == "EXPR") {
        constant = constant->getValue();
    }
    if (constant->getType() == "PROC") {
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
        CodeGenContext::addCodeComment("Inverting the " + operand->getAssignedReg()->getName() +
            " register contents and loading the result into the " + getAssignedReg()->getName() + " register");
        CodeGenContext::addCodeLine("seqz " + getAssignedReg()->getName() + " " + operand->getAssignedReg()->getName());
    }
    if (operName == "-" || operName == "+") {
        if (operName == "-") {
            CodeGenContext::addCodeComment("Negating the  " + operand->getAssignedReg()->getName() +
                " register contents and loading the result into the " + getAssignedReg()->getName() + " register");
            CodeGenContext::addCodeLine("neg " + getAssignedReg()->getName() + " " + operand->getAssignedReg()->getName());
        }
        if (operName == "+") {
            CodeGenContext::addCodeLine("Doing nothing with the " + operand->getAssignedReg()->getName() +
                " register contents and loading the result into the " + getAssignedReg()->getName() + " register");
            CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " " + operand->getAssignedReg()->getName());
        }
    }
}

void ExprExprContext::generateAsmCode() {
    left->generateAsmCode();
    right->generateAsmCode();
    if (assignedRegister == nullptr) {
        if ((left->getType() == "VAR" && !static_cast<ExprVarContext*>(left)->getVariable()->isInMemory()) &&
            right->getType() == "VAR" && !static_cast<ExprVarContext*>(right)->getVariable()->isInMemory()) {
            if (left->getResType() == "REAL" && (operName == "+" || operName == "-" || operName == "*" || operName == "/")) {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
            }
            else {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
            }
        }
        else {
            if (left->getResType() == "REAL" && !(operName == "+" || operName == "-" || operName == "*" || operName == "/")) {
                assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
            }else if (left->getType() == "PROC" || right->getType() == "PROC") {
                if (left->getType() == "PROC" && right->getType() == "PROC") {
                    if (left->getResType() == "REAL") {
                        assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
                    }
                    else {
                        assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
                    }
                }
                else if (left->getType() == "PROC"){
                    if (right->getType() != "VAR" || static_cast<ExprVarContext*>(right)->getVariable()->isInMemory()) {
                        assignedRegister = right->getAssignedReg();
                    }
                    else {
                        if (right->getResType() == "REAL") {
                            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
                        }
                        else {
                            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
                        }
                    }
                }
                else {
                    if (left->getType() != "VAR" || static_cast<ExprVarContext*>(left)->getVariable()->isInMemory()) {
                        assignedRegister = left->getAssignedReg();
                    }
                    else {
                        if (left->getResType() == "REAL") {
                            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FT);
                        }
                        else {
                            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
                        }
                    }
                }
            }
            else {
                if (left->getType() != "VAR" || static_cast<ExprVarContext*>(left)->getVariable()->isInMemory()) {
                    assignedRegister = left->getAssignedReg();
                }
                else {
                    assignedRegister = right->getAssignedReg();
                }
            }
        }
    }

    if (operName == "OR") {
        CodeGenContext::addCodeComment("Performing logical OR operation on the " + left->getAssignedReg()->getName() + 
            " and "+ right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        CodeGenContext::addCodeLine("or " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "&") {
        CodeGenContext::addCodeComment("Performing logical AND operation on the " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        CodeGenContext::addCodeLine("and " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "DIV") {
        CodeGenContext::addCodeComment("Findng the quotient of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        CodeGenContext::addCodeLine("div " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "MOD") {
        CodeGenContext::addCodeComment("Finding the remainder of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        CodeGenContext::addCodeLine("rem " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
    }
    if (operName == "+") {
        CodeGenContext::addCodeComment("Findng the sum of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fadd.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("add " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "-") {
        CodeGenContext::addCodeComment("Findng the difference between " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fsub.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("sub " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "*") {
        CodeGenContext::addCodeComment("Findng the product of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fmul.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("mul " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "/") {
        CodeGenContext::addCodeComment("Findng the quotient of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fdiv.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("div " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "=") {
        CodeGenContext::addCodeComment("Checking the equality of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("feq.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeComment("In order to do that we find the difference between them");
            CodeGenContext::addCodeLine("sub " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeComment("And check if it equals zero");
            CodeGenContext::addCodeLine("seqz " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }
    if (operName == "#") {
        CodeGenContext::addCodeComment("Checking the inequality of " + left->getAssignedReg()->getName() +
            " and " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeComment("In order to do that, we check their equality");
            CodeGenContext::addCodeLine("feq.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeComment("And then negating the result");
            CodeGenContext::addCodeLine("seqz " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeComment("In order to do that we find the difference between them");
            CodeGenContext::addCodeLine("sub " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeComment("And check if it does not equal zero");
            CodeGenContext::addCodeLine("snez " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }
    if (operName == "<") {
        CodeGenContext::addCodeComment("Checking that " + left->getAssignedReg()->getName() +
            " register contents is less then " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("flt.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("slt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == ">") {
        CodeGenContext::addCodeComment("Checking that " + left->getAssignedReg()->getName() +
            " register contents is greater then " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fgt.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeLine("sgt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
    }
    if (operName == "<=") {
        CodeGenContext::addCodeComment("Checking that " + left->getAssignedReg()->getName() +
            " register contents is less then or equal to " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fle.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeComment("In order to do that, we check that " + left->getAssignedReg()->getName() +
                " register contents is greater then " + right->getAssignedReg()->getName() + " register");
            CodeGenContext::addCodeLine("sgt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeComment("And then invert the result");
            CodeGenContext::addCodeLine("seqz " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }

    if (operName == ">=") {
        CodeGenContext::addCodeComment("Checking that " + left->getAssignedReg()->getName() +
            "  register contents is greater then or equal to " + right->getAssignedReg()->getName() + " register contents and loading the result into " + getAssignedReg()->getName() + " register");
        if (left->getResType() == "REAL") {
            CodeGenContext::addCodeLine("fge.d " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
        }
        else {
            CodeGenContext::addCodeComment("In order to do that, we check that " + left->getAssignedReg()->getName() +
                " register contents is less then " + right->getAssignedReg()->getName() + " register");
            CodeGenContext::addCodeLine("slt " + getAssignedReg()->getName() + " " + left->getAssignedReg()->getName() + " " + right->getAssignedReg()->getName());
            CodeGenContext::addCodeComment("And then invert the result");
            CodeGenContext::addCodeLine("seqz " + getAssignedReg()->getName() + " " + getAssignedReg()->getName());
        }
    }

    if ((left->getType() != "VAR" || static_cast<ExprVarContext*>(left)->getVariable()->isInMemory()) && left->getAssignedReg() != assignedRegister) {
        left->FreeReg();
    }
    else if ((left->getType() == "VAR" && static_cast<ExprVarContext*>(left)->getVariable()->isInMemory())) {
        static_cast<ExprVarContext*>(left)->getVariable()->ClearReg();
    }
    if ((right->getType() != "VAR" || static_cast<ExprVarContext*>(right)->getVariable()->isInMemory()) && right->getAssignedReg() != assignedRegister) {
        right->FreeReg();
    }
    else if ((right->getType() == "VAR" && static_cast<ExprVarContext*>(right)->getVariable()->isInMemory())) {
        static_cast<ExprVarContext*>(right)->getVariable()->ClearReg();
    }
}

std::string ExprProcCallContext::getResType() {
    if (proc->getResultType() == nullptr) return "";
    return proc->getResultType()->getTypeName();
}

bool ExprProcCallContext::checkParams() {
    std::vector<ArgVarContext*> formalParams = proc->GetFormalParams();
    if (formalParams.size() != actualParams.size()) return false;
    for (int i = 0; i < formalParams.size();i++) {
        if (formalParams[i]->getType()->getTypeName() != actualParams[i]->getResType()) {
            return false;
        }
        if (formalParams[i]->isVar() && dynamic_cast<ExprVarContext*>(actualParams[i]) == nullptr) {
            return false;
        }
    }
    return true;
}

Register* ExprProcCallContext::getAssignedReg() {
    if (assignedRegister == nullptr) {
        if (proc->getResultType()->getTypeName() == "REAL") {
            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::FAR);
        }
        else {
            assignedRegister = RegisterPool::getInstance().takeFirstFreeReg(RegType::AR);
        }
    }
    return assignedRegister;
}

std::pair<int, std::map<std::string, int>> saveRegsToStack() {
    CodeGenContext::addCodeComment("Saving ra, and used a and fa registers to the stack");
    int totalSize = 4;
    std::map<std::string, int> offsets;
    for (Register* reg : RegisterPool::getInstance().getReversedPool(RegType::AR)) {
        offsets[reg->getName()] = totalSize;
        totalSize += 4;
    }
    for (Register* reg : RegisterPool::getInstance().getReversedPool(RegType::A)) {
        offsets[reg->getName()] = totalSize;
        totalSize += 4;
    }
    for (Register* reg : RegisterPool::getInstance().getReversedPool(RegType::FAR)) {
        offsets[reg->getName()] = totalSize;
        totalSize += 8;
    }
    for (Register* reg : RegisterPool::getInstance().getReversedPool(RegType::FA)) {
        offsets[reg->getName()] = totalSize;
        totalSize += 8;
    }

    CodeGenContext::addCodeComment("Allocating memory on the stack for ra, a, fa registers");
    CodeGenContext::addCodeLine("addi sp sp " + std::to_string(-totalSize));
    CodeGenContext::addCodeLine("sw ra (sp)");

    for (std::pair<std::string, int> pair : offsets) {
        if (pair.first[0] == 'f') {
            CodeGenContext::addCodeLine("fsd " + pair.first + " " + std::to_string(pair.second) + "(sp)");
        }
        else {
            CodeGenContext::addCodeLine("sw " + pair.first + " " + std::to_string(pair.second) + "(sp)");
        }
    }

    RegisterPool::getInstance().pushState(RegType::A);
    RegisterPool::getInstance().pushState(RegType::FA);
    RegisterPool::getInstance().pushState(RegType::AR);
    RegisterPool::getInstance().pushState(RegType::FAR);

    RegisterPool::getInstance().resetToFree(RegType::A);
    RegisterPool::getInstance().resetToFree(RegType::FA);
    RegisterPool::getInstance().resetToFree(RegType::AR);
    RegisterPool::getInstance().resetToFree(RegType::FAR);

    return { totalSize, offsets };
}

void loadRegsFromStack(int totalSize, std::map<std::string, int> offsets, std::vector<std::string> ignore) {
    CodeGenContext::addCodeComment("Restoring ra, à and fa registers from the stack");
    CodeGenContext::addCodeLine("lw ra (sp)");

    for (std::pair<std::string, int> pair : offsets) {
        if (std::find(ignore.begin(), ignore.end(), pair.first) != ignore.end()) continue;
        if (pair.first[0] == 'f') {
            CodeGenContext::addCodeLine("fld " + pair.first + " " + std::to_string(pair.second) + "(sp)");
        }
        else {
            CodeGenContext::addCodeLine("lw " + pair.first + " " + std::to_string(pair.second) + "(sp)");
        }
    }

    CodeGenContext::addCodeComment("Freeing memory on the stack used for storing ra, a, fa registers");
    CodeGenContext::addCodeLine("addi sp sp " + std::to_string(totalSize));

    RegisterPool::getInstance().popState(RegType::A);
    RegisterPool::getInstance().popState(RegType::FA);
    RegisterPool::getInstance().popState(RegType::AR);
    RegisterPool::getInstance().popState(RegType::FAR);
}

void ExprProcCallContext::generateAsmCode() {
    if (proc->getResultType() != nullptr) {
        getAssignedReg();
    } 
    
    std::pair<int, std::map<std::string, int>> offsets;
    if (CodeGenContext::getInstance().isInProc) {
        offsets = saveRegsToStack();
    } 
    else {
        RegisterPool::getInstance().pushState(RegType::A);
        RegisterPool::getInstance().pushState(RegType::FA);
        RegisterPool::getInstance().pushState(RegType::AR);
        RegisterPool::getInstance().pushState(RegType::FAR);

        RegisterPool::getInstance().resetToFree(RegType::A);
        RegisterPool::getInstance().resetToFree(RegType::FA);
        RegisterPool::getInstance().resetToFree(RegType::AR);
        RegisterPool::getInstance().resetToFree(RegType::FAR);
    }
    if (proc->getStackFrameSize() != 0) {
        CodeGenContext::addCodeComment("Allocating the stack memory for the procedure data");
        CodeGenContext::addCodeLine("addi sp sp " + std::to_string(-proc->getStackFrameSize()));
    }

    std::vector<ArgVarContext*> formalParams = proc->GetFormalParams();
    for (int i = 0; i < formalParams.size(); i++) {
        CodeGenContext::addCodeComment("Loading argument number "+std::to_string(i));
        (new AssignmentStatementContext(formalParams[i], actualParams[i]))->generateAsmCode();
        if (!formalParams[i]->isInMemory())
            RegisterPool::getInstance().takeReg(formalParams[i]->getAssignedReg());
    }
    CodeGenContext::addCodeComment("Entering the procedure");
    CodeGenContext::addCodeLine("jal " + proc->getLabel());

    std::vector<std::string> ignore;

    for (int i = 0; i < formalParams.size(); i++) {
        if (formalParams[i]->isVar()) {
            CodeGenContext::addCodeComment("Formal parameter \"" + formalParams[i]->getName() + " is marked as VAR, so");
            CodeGenContext::addCodeComment("Moving the last register value of the parameter to the passed argument register");
            (new AssignmentStatementContext(dynamic_cast<ExprVarContext*>(actualParams[i])->getVariable(), new ExprVarContext(formalParams[i])))->generateAsmCode();
        }
    } 

    if (proc->getResultType() != nullptr) {
        CodeGenContext::addCodeComment("Moving the value returned by the procedure into "+getAssignedReg()->getName() + " register");
        if (proc->getResultType()->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("fmv.d " + getAssignedReg()->getName() + " fa1");
        }
        else {
            CodeGenContext::addCodeLine("mv " + getAssignedReg()->getName() + " a1");
        }
        ignore.push_back(getAssignedReg()->getName());
    }

    if (proc->getStackFrameSize() != 0) {
        CodeGenContext::addCodeComment("Freeing the stack memory reserved for the procedure data");
        CodeGenContext::addCodeLine("addi sp sp " + std::to_string(proc->getStackFrameSize()));
    }
    if (CodeGenContext::getInstance().isInProc) {
        loadRegsFromStack(offsets.first, offsets.second, ignore);
    }
    else {
        RegisterPool::getInstance().popState(RegType::A);
        RegisterPool::getInstance().popState(RegType::FA);
        RegisterPool::getInstance().popState(RegType::AR);
        RegisterPool::getInstance().popState(RegType::FAR);
    }
}



void ExprProcCallContext::debugOut() {
    std::cout << proc->getProcedureName() + "(...)";
    std::cout << " )";
}