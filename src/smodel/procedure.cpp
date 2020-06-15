#include "procedure.h"

#include "variable.h"

// Конструктор для создания первоначального процедурного контекста
ProcContext::ProcContext(Module &m): resultType(nullptr) {}

// Вывод общей отладочной информации о переменной
void ProcContext::debugOut() {
    std::cout << "PROCEDURE ";
    // Добавление информации о типе переменной
    std::cout << "RESULT ";
    resultType->debugOut();
}

