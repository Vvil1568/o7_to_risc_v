#include "procedure.h"

#include "variable.h"

// Вывод общей отладочной информации о переменной
void ProcContext::debugOut() {
    std::cout << "PROCEDURE ";
    // Добавление информации о типе переменной
    std::cout << "RESULT ";
    resultType->debugOut();
}
// Добавление именованного артефакта
void ProcContext::AddNamedArtefact(std::string name, Context* context) {
    NamedArtefact* tmpNamed = new NamedArtefact{ name, context, false };
    namedArtefacts.push_back(tmpNamed);
    // Контекст получает обратную ссылку на именованный артефакт
    context->setNamedArtefact(tmpNamed);
}

