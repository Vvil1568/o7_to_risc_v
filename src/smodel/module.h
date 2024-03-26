#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>
#include <vector>

#include "context.h"
#include "const.h"
#include "type.h"
#include "statement.h"
#include "registerpool.h"
#include "codegencontext.h"

// Класс определяющий данные, непосредственно доступные для модуля и процедур
class CommonData {
public:
    void SetStatementSequence(std::vector<StatementContext*> statements);
    std::vector<StatementContext*> GetStatementSequence();
protected:
    static std::string moduleName;  // имя модуля
    // Список артефактов с зарезервированными именами
    static std::vector<NamedArtefact*> reservedNamedArtefacts;
    // Список именованных артефактов, порождаемых в модуле
    static std::vector<NamedArtefact*> namedArtefacts;
private:
    std::vector<StatementContext*> statements;
};

// Класс, определяющий модуль семантической модели.
// Содержит все данные, описывающие артефакты языка программирования
class Module: public CommonData {
    friend class Creator;
public:
    Module();

    // Установка имени модуля
    void setModuleName(std::string mn) { moduleName = mn; }
    // Вывод отладочной информации о модуле
    void debugOut();

    // Добавление именованного артефакта
    void AddNamedArtefact(std::string name, Context* context, bool access = false);

    void CompileModule();

    TypeContext* GetTypeFromName(std::string name);
    ConstContext* GetConstFromName(std::string name);
    VarContext* GetVarFromName(std::string name);
};

#endif // MODULE_H
