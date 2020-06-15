#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>
#include <vector>

#include "context.h"

// Класс определяющий данные, непосредственно доступные для модуля и процедур
class CommonData {
protected:
    static std::string moduleName;  // имя модуля
    // Список артефактов с зарезервированными именами
    static std::vector<NamedArtefact*> reservedNamedArtefacts;
    // Список именованных артефактов, порождаемых в модуле
    static std::vector<NamedArtefact*> namedArtefacts;
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

///private:
};

#endif // MODULE_H
