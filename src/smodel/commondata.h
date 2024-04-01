#ifndef COMMONDATA_H
#define COMMONDATA_H

#include <vector>
#include <string>
#include "variable.h"
#include "const.h"
#include "statement.h"

class StatementContext;

// Класс определяющий данные, непосредственно доступные для модуля и процедур
class CommonData {
public:
    void SetStatementSequence(std::vector<StatementContext*> statements);
    std::vector<StatementContext*> GetStatementSequence();
    // Добавление именованного артефакта
    void AddNamedArtefact(std::string name, Context* context, bool access = false);
    // Получение типа по его названию
    TypeContext* GetTypeFromName(std::string name);
    // Получение константы по ее названию
    ConstContext* GetConstFromName(std::string name);
    // Получение переменной по ее названию
    VarContext* GetVarFromName(std::string name);
    // Получение списка всех именованных артефактов
    std::vector<NamedArtefact*> getNamedArtefacts() { return namedArtefacts; }
protected:
    std::string name = "";  // имя блока данных
    // Список артефактов с зарезервированными именами
    std::vector<NamedArtefact*> reservedNamedArtefacts;
    // Список именованных артефактов, порождаемых в модуле
    std::vector<NamedArtefact*> namedArtefacts;
    std::vector<StatementContext*> statements;
};
#endif