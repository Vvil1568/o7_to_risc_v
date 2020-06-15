#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "module.h"
#include "variable.h"

// Класс, задающий контекст для процедуры
class ProcContext: public Context, CommonData {
public:
    // Конструктор для создания переменной заданного типа
    ProcContext(Module& m);

    // Получение информации о типе переменой
    TypeContext* getResultType() { return resultType; }
    // Установка типа переменной
    void setResultType(TypeContext* type) { resultType = type; }

    // Вывод отладочной информации о базовом типе
    virtual void debugOut();
private:
    TypeContext* resultType;   // Тип результата
};


#endif // PROCEDURE_H
