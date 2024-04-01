#include <string>

#include "const.h"

// Вывод отладочной информации об общем контексте константы
void ConstContext::debugOut() {
    std::cout << "CONST ";
}


// Вывод отладочной информации о контексте ошибки
void ConstErrContext::debugOut() {
    std::cout << "CONST ERR";
}

// Вывод отладочной информации о контексте NIL
void ConstNilContext::debugOut() {
    std::cout << "CONST NIL";
}

// Вывод отладочной информации о контексте NIL
void ConstExprContext::debugOut() {
    std::cout << "CONST EXPR";
}

// Вывод отладочной информации о контексте NIL
void ConstUnarExprContext::debugOut() {
    std::cout << "CONST UNAR EXPR";
}

// Вывод отладочной информации о целочисленной константе
void ConstIntContext::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}

// Вывод отладочной информации о вещественной константе
void ConstRealContext::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}

// Вывод отладочной информации о вещественной константе
void ConstStringContext::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}

// Вывод отладочной информации о булевой константе
void ConstBooleanContext::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}
//TODO debug out о выражениях

ConstContext* ConstUnarExprContext::getValue() {
    if (operand->getType() == "EXPR") {
        operand = operand->getValue();
    }
    if (operand->getType() == "ERR") {
        return operand;
    }
    if (operName == "~") {
        if (operand->getType() != "BOOL") {
            return new ConstErrContext("Unsupported operation " + operName + " for type " + operand->getType());
        }
        bool val = (static_cast<ConstBooleanContext*>(operand))->getBoolValue();
        return new ConstBooleanContext(!val);
    }
    if (operName == "-" || operName == "+") {
        if (operand->getType() != "INT" && operand->getType() != "REAL") {
            return new ConstErrContext("Unsupported operation " + operName + " for type " + operand->getType());
        }
        if (operand->getType() == "INT") {
            int val = (static_cast<ConstIntContext*>(operand))->getIntValue();
            if (operName == "-") return new ConstIntContext(-val);
            if (operName == "+") return new ConstIntContext(val);
        }
        else {
            double val = (static_cast<ConstRealContext*>(operand))->getRealValue();
            if (operName == "-") return new ConstRealContext(-val);
            if (operName == "+") return new ConstRealContext(val);
        }
    }
    return new ConstErrContext("Unsupported operation " + operName + " for type " + operand->getType());
}

ConstContext* ConstExprContext::getValue() {
    if (left->getType() == "EXPR") {
        left = left->getValue();
    }
    if (right->getType() == "EXPR") {
        right = right->getValue();
    }
    if (left->getType() == "ERR") {
        return left;
    }
    if (right->getType() == "ERR") {
        return right;
    }
    if (operName == "OR" || operName == "&") {
        if (left->getType() != "BOOL" || right->getType() != "BOOL") {
            return new ConstErrContext("Unsupported operation " + operName + " between types "+ left->getType()+" and "+ right->getType());
        }
        bool val1 = (static_cast<ConstBooleanContext*>(left))->getBoolValue();
        bool val2 = (static_cast<ConstBooleanContext*>(right))->getBoolValue();
        if (operName == "OR") return new ConstBooleanContext(val1 | val2);
        if (operName == "&") return new ConstBooleanContext(val1 & val2);
    }
    if (operName == "DIV" || operName == "MOD") {
        if (left->getType() != "INT" || right->getType() != "INT") {
            return new ConstErrContext("Unsupported operation " + operName + " between types " + left->getType() + " and " + right->getType());
        }
        int val1 = (static_cast<ConstIntContext*>(left))->getIntValue();
        int val2 = (static_cast<ConstIntContext*>(right))->getIntValue();
        if (operName == "MOD") return new ConstIntContext(val1 % val2);
        if (operName == "DIV") return new ConstIntContext(val1 / val2);
    }
    if (operName == "+" || operName == "-" || operName == "*" || operName == "/") {
        if ((left->getType() != right->getType())) {
            return new ConstErrContext("Unsupported operation " + operName + " between different types " + left->getType() + " and " + right->getType());
        }
        if ((left->getType() == "REAL")) {
            double val1 = (static_cast<ConstRealContext*>(left))->getRealValue();
            double val2 = (static_cast<ConstRealContext*>(right))->getRealValue();
            if (operName == "+") return new ConstRealContext(val1 + val2);
            if (operName == "-") return new ConstRealContext(val1 - val2);
            if (operName == "*") return new ConstRealContext(val1 * val2);
            if (operName == "/") return new ConstRealContext(val1 / val2);
        } else if ((left->getType() == "INT")) {
            int val1 = (static_cast<ConstIntContext*>(left))->getIntValue();
            int val2 = (static_cast<ConstIntContext*>(right))->getIntValue();
            if (operName == "+") return new ConstIntContext(val1 + val2);
            if (operName == "-") return new ConstIntContext(val1 - val2);
            if (operName == "*") return new ConstIntContext(val1 * val2);
            if (operName == "/") return new ConstIntContext(val1 / val2);
        }
        else {
            return new ConstErrContext("Unsupported operation " + operName + " between " + left->getType() + "s");
        }
    }
    if (operName == "=" || operName == "#") {
        if ((left->getType() != right->getType())) {
            return new ConstErrContext("Unsupported operation " + operName + " between different types " + left->getType() + " and " + right->getType());
        }
        if ((left->getType() == "REAL")) {
            double val1 = (static_cast<ConstRealContext*>(left))->getRealValue();
            double val2 = (static_cast<ConstRealContext*>(right))->getRealValue();
            if (operName == "=") return new ConstBooleanContext(val1 == val2);
            if (operName == "#") return new ConstBooleanContext(val1 != val2);
        }
        if ((left->getType() == "INT")) {
            int val1 = (static_cast<ConstIntContext*>(left))->getIntValue();
            int val2 = (static_cast<ConstIntContext*>(right))->getIntValue();
            if (operName == "=") return new ConstBooleanContext(val1 == val2);
            if (operName == "#") return new ConstBooleanContext(val1 != val2);
        }
        if ((left->getType() == "BOOL")) {
            bool val1 = (static_cast<ConstBooleanContext*>(left))->getBoolValue();
            bool val2 = (static_cast<ConstBooleanContext*>(right))->getBoolValue();
            if (operName == "=") return new ConstBooleanContext(val1 == val2);
            if (operName == "#") return new ConstBooleanContext(val1 != val2);
        }
        if ((left->getType() == "STRING")) {
            std::string val1 = (static_cast<ConstStringContext*>(left))->getStrValue();
            std::string val2 = (static_cast<ConstStringContext*>(right))->getStrValue();
            if (operName == "=") return new ConstBooleanContext(val1 == val2);
            if (operName == "#") return new ConstBooleanContext(val1 != val2);
        }
        return new ConstErrContext("Unsupported operation " + operName + " between " + left->getType() + "s");
    }
    if (operName == ">" || operName == "<" || operName == "<=" || operName == ">=") {
        if ((left->getType() != right->getType())) {
            return new ConstErrContext("Unsupported operation " + operName + " between different types " + left->getType() + " and " + right->getType());
        }
        if ((left->getType() == "REAL")) {
            double val1 = (static_cast<ConstRealContext*>(left))->getRealValue();
            double val2 = (static_cast<ConstRealContext*>(right))->getRealValue();
            if (operName == ">") return new ConstRealContext(val1 > val2);
            if (operName == "<") return new ConstRealContext(val1 < val2);
            if (operName == "<=") return new ConstRealContext(val1 <= val2);
            if (operName == ">=") return new ConstRealContext(val1 >= val2);
        }
        else if ((left->getType() == "INT")) {
            int val1 = (static_cast<ConstIntContext*>(left))->getIntValue();
            int val2 = (static_cast<ConstIntContext*>(right))->getIntValue();
            if (operName == ">") return new ConstRealContext(val1 > val2);
            if (operName == "<") return new ConstRealContext(val1 < val2);
            if (operName == "<=") return new ConstRealContext(val1 <= val2);
            if (operName == ">=") return new ConstRealContext(val1 >= val2);
        }
        else {
            return new ConstErrContext("Unsupported operation " + operName + " between " + left->getType() + "s");
        }
    }
    return new ConstErrContext("Unsupported operator: "+operName);
}

/*
// Вывод отладочной информации о константе
template<typename T>
void ConstValueContext<T>::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}
*/