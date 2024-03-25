#include "variable.h"

// Конструктор для создания переменной заданного типа
VarContext::VarContext(std::string name, TypeContext* type): context(type) {
    varName = name;
}

// Вывод общей отладочной информации о переменной
void VarContext::debugOut() {
    std::cout << "VAR ";
    // Добавление информации о типе переменной
    context->debugOut();
}

