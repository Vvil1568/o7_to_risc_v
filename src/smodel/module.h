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
#include "procedure.h"
#include "stackcontroller.h"
#include "commondata.h"

// Класс, определяющий модуль семантической модели.
// Содержит все данные, описывающие артефакты языка программирования
class Module: public CommonData {
    friend class Creator;
public:
    Module();

    // Установка имени модуля
    void setModuleName(std::string mn) { name = mn; }
    // Вывод отладочной информации о модуле
    void debugOut();
    void CompileModule();
};

#endif // MODULE_H
