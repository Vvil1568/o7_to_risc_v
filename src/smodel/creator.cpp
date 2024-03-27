#include "creator.h"
#include "import.h"
#include "const.h"
#include "type.h"

Creator::Creator()
{

}


// Создание контекста импорта
ImportContext* Creator::CreateImportContext(std::string importedName, std::string alias) {
    return new ImportContext(importedName, alias);
}

// Создание целочисленной константы
ConstContext* Creator::CreateConstInt(int v) {
    return new ConstIntContext(v);
}

// Создание действительной константы
ConstContext* Creator::CreateConstReal(double v) {
    return new ConstRealContext(v);
}

// Создание булевской константы
ConstContext* Creator::CreateConstBool(bool v) {
    return new ConstBooleanContext(v);
}

// Создание строковой константы
ConstContext* Creator::CreateConstString(std::string v) {
    return new ConstStringContext(v);
}

// Создание константы NIL
ConstContext* Creator::CreateConstNil() {
    return new ConstNilContext();
}

// Создание константного унарного выражения
ConstContext* Creator::CreateConstUnarExpr(ConstContext* operand, std::string opName) {
    return new ConstUnarExprContext(operand, opName);
}
// Создание константного унарного выражения
ConstContext* Creator::CreateConstExpr(ConstContext* left, ConstContext* right, std::string opName) {
    return new ConstExprContext(left, right, opName);
}

// Создание целочисленного литерала
ExprContext* Creator::CreateIntExpr(int v) {
    return new ExprIntContext(v);
}
// Создание действительного литерала
ExprContext* Creator::CreateRealExpr(double v) {
    return new ExprRealContext(v);
}
// Создание булевского литерала
ExprContext* Creator::CreateBoolExpr(bool v) {
    return new ExprBooleanContext(v);
}
// Создание строкового литерала
ExprContext* Creator::CreateStringExpr(std::string v) {
    return new ExprStringContext(v);
}
// Создание литерала NIL
ExprContext* Creator::CreateNilExpr() {
    return new ExprNilContext();
}
// Создание унарного выражения
ExprContext* Creator::CreateUnarExpr(ExprContext* operand, std::string opName) {
    return new ExprUnarExprContext(operand, opName);
}
// Создание выражения
ExprContext* Creator::CreateExpr(ExprContext* left, ExprContext* right, std::string opName) {
    return new ExprExprContext(left, right, opName);
}
// Создание выражения значения константы
ExprContext* Creator::CreateConstValueExpr(ConstContext* constant) {
    return new ExprConstContext(constant);
}
// Создание выражения значения переменной
ExprContext* Creator::CreateVarValueExpr(VarContext* variable) {
    return new ExprVarContext(variable);
}

// Создание целочисленного типа
TypeIntegerContext* Creator::CreateTypeInt() {
    return new TypeIntegerContext();
}

// Создание действительного типа
TypeRealContext* Creator::CreateTypeReal() {
    return new TypeRealContext();
}

// Создание булевского типа
TypeBoolContext* Creator::CreateTypeBool()  {
    return new TypeBoolContext();
}

// Создание символьного типа
TypeCharContext* Creator::CreateTypeChar() {
    return new TypeCharContext();
}

// Создание множественного типа
TypeSetContext* Creator::CreateTypeSet() {
    return new TypeSetContext();
}

// Создание типа Запись (пустой оболочки)
TypeRecordContext* Creator::CreateTypeRecord(TypeRecordContext* parent) {
    return new TypeRecordContext(parent);
}

// Создание типа Запись (пустой оболочки)
TypeArrayContext* Creator::CreateTypeArray(TypeContext* type, int size) {
    return new TypeArrayContext(type, size);
}

// Создание типа-указателя на запись
TypePointerContext* Creator::CreateTypePointer(TypeRecordContext* r) {
    return new TypePointerContext(r);
}

// Создание переменной с установкой соответствующего типа
VarContext* Creator::CreateVariable(std::string name, TypeContext* type) {
    return new VarContext(name, type);
}

StatementContext* Creator::CreateAssignmentStatement(VarContext* var, ExprContext* expr) {
    return new AssignmentStatementContext(var, expr);
}

StatementContext* Creator::CreateIfStatement(std::vector<ConditionalStatementsBlock> statementBlocks) {
    return new IfStatementContext(statementBlocks);
}

StatementContext* Creator::CreateCaseStatement(std::vector<ConditionalStatementsBlock> statementBlocks) {
    return new CaseStatementContext(statementBlocks);
}

StatementContext* Creator::CreateWhileStatement(std::vector<WhileStatementsBlock> statementBlocks) {
    return new WhileStatementContext(statementBlocks);
}

StatementContext* Creator::CreateRepeatStatement(ExprContext* condition, std::vector<StatementContext*> statements) {
    return new RepeatStatementContext(condition, statements);
}

StatementContext* Creator::CreateForStatement(StatementContext* init, ExprContext* condition, StatementContext* step, std::vector<StatementContext*> statements) {
    return new ForStatementContext(init, condition, step, statements);
}