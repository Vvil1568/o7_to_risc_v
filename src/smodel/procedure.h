#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "module.h"
#include "variable.h"

// Класс, задающий контекст для процедуры
class ProcContext: public TypeContext, CommonData {
public:
    // Конструктор для создания переменной заданного типа
    ProcContext() { typeSize = sizeof(int); } //размер процедуры - размер метки на ее начало

    // Получение информации о типе переменой
    TypeContext* getResultType() { return resultType; }
    // Установка типа переменной
    void setResultType(TypeContext* type) { resultType = type; }
    // Установка имени процедуры
    void setProcedureName(std::string pn) { moduleName = pn; }
    // Добавление именованного артефакта
    void AddNamedArtefact(std::string name, Context* context);


    // Вывод отладочной информации о базовом типе
    virtual void debugOut();
private:
    TypeContext* resultType;   // Тип результата
};


#endif // PROCEDURE_H
