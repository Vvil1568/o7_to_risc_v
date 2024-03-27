#ifndef CREATOR_H
#define CREATOR_H

#include <string>

#include "context.h"
#include "import.h"
#include "const.h"
#include "type.h"
#include "variable.h"
#include "module.h"
#include "procedure.h"
#include "expr.h"
#include "statement.h"

// Класс, обеспечивающий создание различных артефактов
class Creator
{
public:
    Creator();

    // Создание контекста импорта
    ImportContext* CreateImportContext(std::string importedName, std::string alias);

    // Создание целочисленной константы
    ConstContext* CreateConstInt(int v);
    // Создание действительной константы
    ConstContext* CreateConstReal(double v);
    // Создание булевской константы
    ConstContext* CreateConstBool(bool v);
    // Создание строковой константы
    ConstContext* CreateConstString(std::string v);
    // Создание константы NIL
    ConstContext* CreateConstNil();
    // Создание константного унарного выражения
    ConstContext* CreateConstUnarExpr(ConstContext* operand, std::string opName);
    // Создание константного выражения
    ConstContext* CreateConstExpr(ConstContext* left, ConstContext* right, std::string opName);

    // Создание целочисленного литерала
    ExprContext* CreateIntExpr(int v);
    // Создание действительного литерала
    ExprContext* CreateRealExpr(double v);
    // Создание булевского литерала
    ExprContext* CreateBoolExpr(bool v);
    // Создание строкового литерала
    ExprContext* CreateStringExpr(std::string v);
    // Создание литерала NIL
    ExprContext* CreateNilExpr();
    // Создание унарного выражения
    ExprContext* CreateUnarExpr(ExprContext* operand, std::string opName);
    // Создание выражения
    ExprContext* CreateExpr(ExprContext* left, ExprContext* right, std::string opName);
    // Создание выражения значения константы
    ExprContext* CreateConstValueExpr(ConstContext* constant);
    // Создание выражения значения переменной
    ExprContext* CreateVarValueExpr(VarContext* variable);

    // Создание целочисленного типа
    TypeIntegerContext* CreateTypeInt();
    // Создание действительного типа
    TypeRealContext* CreateTypeReal();
    // Создание булевского типа
    TypeBoolContext* CreateTypeBool();
    // Создание символьного типа
    TypeCharContext* CreateTypeChar();
    // Создание множественного типа
    TypeSetContext* CreateTypeSet();

    // Создание типа Запись (пустой оболочки)
    TypeRecordContext* CreateTypeRecord(TypeRecordContext* parent = nullptr);

    // Создание типа-указателя на запись
    TypePointerContext* CreateTypePointer(TypeRecordContext* r);

    // Создание типа-массива
    TypeArrayContext* CreateTypeArray(TypeContext* type, int size);

    // Создание переменной с установкой соответствующего типа
    VarContext* CreateVariable(std::string name, TypeContext* type);

    StatementContext* CreateAssignmentStatement(VarContext* var, ExprContext* expr);

    StatementContext* CreateIfStatement(std::vector<ConditionalStatementsBlock> statementBlocks);
    StatementContext* CreateCaseStatement(std::vector<ConditionalStatementsBlock> statementBlocks);
    StatementContext* CreateWhileStatement(std::vector<WhileStatementsBlock> statementBlocks);
    StatementContext* CreateRepeatStatement(ExprContext* condition, std::vector<StatementContext*> statements);
    StatementContext* CreateForStatement(StatementContext* init, ExprContext* condition, StatementContext* step, std::vector<StatementContext*> statements);
};

#endif // CREATOR_H
