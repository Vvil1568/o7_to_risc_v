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