#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "commondata.h"
#include "variable.h"
#include "type.h"

class ExprContext;

// Класс, задающий контекст для процедуры
class ProcContext: public TypeContext, public CommonData {
public:
    // Конструктор для создания переменной заданного типа
    ProcContext() { typeSize = sizeof(int); } //размер процедуры - размер метки на ее начало

    // Получение информации о типе переменой
    TypeContext* getResultType() { return resultType; }
    // Установка типа переменной
    void setResultType(TypeContext* type) { resultType = type; }
    // Установка имени процедуры
    void setProcedureName(std::string pn) { name = pn; }
    // Установка имени процедуры
    std::string getProcedureName() { return name; }
    // Добавление именованного артефакта
    void AddNamedArtefact(std::string name, Context* context);
    // Установка возвращающего выражения процедуры
    void SetReturnExpr(ExprContext* retExpr) { returnExpr = retExpr; }
    // Получение списка формальных параметров процедуры
    std::vector<ArgVarContext*> GetFormalParams();
    // Вычисление кадра стека
    void ComputeStackFrame();
    // Генерация ассемблерного кода
    void GenerateAsmCode();
    // Вывод отладочной информации о базовом типе
    virtual void debugOut();
    // Получение размера кадра стека для процедуры
    int getStackFrameSize() { return stackFrameSize; }
    // Получение названия метки для процедуры
    std::string getLabel() { return label; }
    // Получение названия метки для процедуры
    void setLabel(std::string labelName) { label = labelName; }

    void setResReg(Register* reg) { resReg = reg; }
private:
    bool isCompute = false;
    bool isASMed = false;
    int stackFrameSize = 0;
    std::string label = "";
    Register* resReg = nullptr;
    ExprContext* returnExpr = nullptr;
    TypeContext* resultType = nullptr;   // Тип результата
};


#endif // PROCEDURE_H
