//#include "error.h"
#include "parser.h"

// Список ключевых и зарезервированных слов,
// которые не могут использоваться в качестве идентификатора
namespace {
    std::vector<std::string> keyWords = {
        "ARRAY",
        "BEGIN",
        "BY",
        "CASE",
        "CONST",
        "DIV",
        "DO",
        "ELSE",
        "ELSIF",
        "END",
        "FALSE",
        "FOR",
        "IMPORT",
        "IF",
        "IN",
        "IS",
        "MOD",
        "MODULE",
        "NIL",
        "OF",
        "OR",
        "POINTER",
        "PROCEDURE",
        "RECORD",
        "REPEAT",
        "RETURN",
        "THEN",
        "TO",
        "TRUE",
        "TYPE",
        "UNTIL",
        "VAR",
        "WHILE"
    };
}

// Начальные установки параметров компилятора и его запуск
void Compile(const char* str) {
    ModuleCompiler mc(str);
    
    std::cout << "***** MODULE *****" << std::endl;
    std::cout << str << std::endl;

    std::cout << "***** COMPILER STARTED *****" << std::endl;
    if(mc.isModule()) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "FAIL" << std::endl;
    }
    mc.getModule().CompileModule();
    freopen("res.s", "w", stdout);
    std::vector<std::string> asmData = CodeGenContext::getInstance().data;
    for (std::string line : asmData) {
        std::cout << line << std::endl;
    }
    std::vector<std::string> asmCode = CodeGenContext::getInstance().code;
    for (std::string line : asmCode) {
        std::cout << line << std::endl;
    }
    fclose(stdout);
}

// Конструктор, формирующий начальные установки параметров компилятора
ModuleCompiler::ModuleCompiler(const char* str): moduleStr{str},
    pos{0}, line{1}, column{1}
{}

Module ModuleCompiler::getModule() {
    return curModule;
}

//-----------------------------------------------------------------------------
// Module = MODULE ident ";" [ImportList] DeclarationSequence
//          [BEGIN StatementSequence] END ident "." .
bool ModuleCompiler::isModule() {
//_0:
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
    ignore();
    if(isKeyWord("MODULE")) {
        ignore();
        goto _1;
    }
    return erMessage("The Module must start using key word MODULE");
_1:
    if(isIdent()) {
        curModule.setModuleName(lexValue);
        goto _2;
    }
    return erMessage("It's not the name of Module");
_2:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Semicolon expected");
_3:
    if(isImportList()) {
        goto _4;
    }
    if(isDeclarationSequence()) {
        goto _5;
    }
    if(isKeyWord("BEGIN")) {
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("ImportList or DeclarationSequence or StatementSequence or END expected");
_4:
    if(isDeclarationSequence()) {
        goto _5;
    }
    if(isKeyWord("BEGIN")) {
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("DeclarationSequence or StatementSequence or END expected");
_5:
    if(isKeyWord("BEGIN")) {
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("BEGIN or END expected");
_6:
    stSeqCheckRes = isStatementSequence();
    if (stSeqCheckRes.first) {
        curModule.SetStatementSequence(stSeqCheckRes.second);
        goto _7;
    }
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("StatementSequence or END expected");
_7:
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("END expected");
_8:
    if(isIdent()) {
        goto _9;
    }
    return erMessage("It's not the name of Module");
_9:
    if(isSymbol(moduleStr[pos], '.')) {
        ++pos;
        ++column;
        ignore();
        goto _10;
    }
    return erMessage("Point expected");
_10:
    if(isEndOfString()) {
        goto _end;
    }
    return erMessage("End of Module expected");
_end:
    /// Тестовый вывод (закомментировать после отработки)
    ///testMessage("It's Module");
    return true;
}

//-----------------------------------------------------------------------------
// ImportList = IMPORT import {"," import} ";".
// import = ident [":=" ident].
bool ModuleCompiler::isImportList() {
    std::string name;
    std::string alias;
//_0:
    if(isKeyWord("IMPORT")) {
        goto _1;
    }
    return false;
_1:
    name = "";
    alias = "";
    if(isIdent()) {
        alias = lexValue;
        goto _2;
    }
    return erMessage("It's not the imported name or alias");
_2:
    if(isSymbol(moduleStr[pos], ',')) {
        curModule.AddNamedArtefact(alias, creator.CreateImportContext(alias, alias));
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isAssignSymbol()) {
        goto _3;
    }
    return erMessage("It's not comma or  semicolon the imported name or alias or asignment");
_3:
    if(isIdent()) {
        name = lexValue;
        goto _4;
    }
    return erMessage("It's not the imported name");
_4:
    if(isSymbol(moduleStr[pos], ',')) {
        curModule.AddNamedArtefact(alias, creator.CreateImportContext(name, alias));
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return erMessage("It's not ',' or ';'");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// DeclarationSequence = [CONST {ConstDeclaration ";"}]
//    [TYPE {TypeDeclaration ";"}]
//    [VAR {VariableDeclaration ";"}]
//    {ProcedureDeclaration ";"}.
bool ModuleCompiler::isDeclarationSequence() {
//_0:
    if(isKeyWord("CONST")) {
        goto _1;
    }
    if(isKeyWord("TYPE")) {
        goto _4;
    }
    if(isKeyWord("VAR")) {
        goto _7;
    }
    if(isProcedureDeclaration()) {
        goto _10;
    }
    return false;
_1:
    if(isConstDeclaration()) {
        goto _2;
    }
    return erMessage("It's not the Declaration of Constants");
_2:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Semicolon expected");
_3:
    if(isConstDeclaration()) {
        goto _2;
    }
    if(isKeyWord("TYPE")) {
        goto _4;
    }
    if(isKeyWord("VAR")) {
        goto _7;
    }
    if(isProcedureDeclaration()) {
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_4:
    if(isTypeDeclaration()) {
        goto _5;
    }
    return erMessage("It's not the Type Declaration");
_5:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _6;
    }
    return erMessage("Semicolon expected");
_6:
    if(isTypeDeclaration()) {
        goto _5;
    }
    if(isKeyWord("VAR")) {
        goto _7;
    }
    if(isProcedureDeclaration()) {
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_7:
    if(isVariableDeclaration()) {
        goto _8;
    }
    return erMessage("It's not the Declaration of Variables");
_8:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _9;
    }
    return erMessage("Semicolon expected");
_9:
    if(isVariableDeclaration()) {
        goto _8;
    }
    if(isProcedureDeclaration()) {
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_10:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _11;
    }
    return erMessage("Semicolon expected");
_11:
    if(isProcedureDeclaration()) {
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstDeclaration = identdef "=" ConstExpression.
// identdef = ident ["*"].
// ConstExpression = expression.
bool ModuleCompiler::isConstDeclaration() {
//_0:
    std::string name = "";
    bool access = false;
    if(isIdentdef()) {
        name = lexValue;
        if (name.back() == '*') {
            name = name.substr(0, name.length() - 1);
            access = true;
        }
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '=')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    return erMessage("Symbol '=' expected");
_2:
    std::pair<bool, ConstContext*> checkRes1 = isConstExpression();
    if(checkRes1.first) {
        ConstContext* result = checkRes1.second->getValue();
        if (result->getType() == "ERR") {
            return erMessage("Error while constant evaluation: "+dynamic_cast<ConstErrContext*>(result)->getErrMsg());
        }
        curModule.AddNamedArtefact(name, result, access);
        goto _end;
    }
    return erMessage("Constant Expression expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstExpression = SimpleConstExpression [relation SimpleConstExpression].
std::pair<bool, ConstContext*> ModuleCompiler::isConstExpression() {
//_0:
    std::string operName;
    std::pair<bool, ConstContext*> checkRes1 = isSimpleConstExpression();
    ConstContext* firstOperand;
    if (checkRes1.first) {
        firstOperand = checkRes1.second;
        goto _1;
    }
    return { false, nullptr };
_1:
    if(isRelation()) {
        operName = lexValue;
        goto _2;
    }
    goto _end;
_2:
    std::pair<bool, ConstContext*> checkRes2 = isSimpleConstExpression();
    if(checkRes2.first) {
        firstOperand = creator.CreateConstExpr(firstOperand, checkRes2.second, operName);
        goto _end;
    }
    return { erMessage("Simple Constant Expression expected"), nullptr };
_end:
    return { true, firstOperand };
}

//-----------------------------------------------------------------------------
// SimpleConstExpression = ["+" | "-"] ConstTerm {AddOperator ConstTerm}.
std::pair<bool, ConstContext*> ModuleCompiler::isSimpleConstExpression() {
//_0:
    std::string unarOpName = "";
    std::string operName;
    ConstContext* firstOperand = nullptr;
    if(isSymbol(moduleStr[pos], '+')) {
        ++pos;
        ++column;
        ignore();
        unarOpName = "+";
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '-')) {
        ++pos;
        ++column;
        ignore();
        unarOpName = "-";
        goto _1;
    }
    std::pair<bool, ConstContext*> checkRes1 = isConstTerm();
    if(checkRes1.first) {
        firstOperand = checkRes1.second;
        goto _2;
    }
    return { false, nullptr };
_1:
    std::pair<bool, ConstContext*> checkRes2 = isConstTerm();
    if (checkRes2.first) {
        if (firstOperand != nullptr) {
            firstOperand = creator.CreateConstExpr(firstOperand, checkRes2.second, operName);
        }
        else {
            firstOperand = creator.CreateConstUnarExpr(checkRes2.second, unarOpName);
        }
        goto _2;
    }
    return { erMessage("Constant Term expected"), nullptr };
_2:
    if(isAddOperator()) {
        operName = lexValue;
        goto _1;
    }
    goto _end;
_end:
    return { true, firstOperand };
}

//-----------------------------------------------------------------------------
// ConstTerm = ConstFactor {MulOperator ConstFactor}.
std::pair<bool, ConstContext*> ModuleCompiler::isConstTerm() {
//_0:
    std::pair<bool, ConstContext*> checkRes1 = isConstFactor();
    std::string operName;
    ConstContext* firstOperand;
    if (checkRes1.first) {
        firstOperand = checkRes1.second;
        goto _1;
    }
    return { false, nullptr };
_1: 
    if(isMulOperator()) {
        operName = lexValue;
        goto _2;
    }
    goto _end;
_2:
    std::pair<bool, ConstContext*> checkRes2 = isConstFactor();
    if (checkRes2.first) {
        firstOperand = creator.CreateConstExpr(firstOperand, checkRes2.second, operName);
        goto _1;
    }
    return { erMessage("Constant Factor expected"), nullptr };
_end:
    return { true, firstOperand };
}

//-----------------------------------------------------------------------------
// ConstFactor = number | string | NIL | TRUE | FALSE |
//             ConstSet | designator [ActualParameters] | "(" ConstExpression ")" | "~" ConstFactor.
// number = integer | real
std::pair<bool, ConstContext*> ModuleCompiler::isConstFactor() {
//_0:
    ConstContext* factor;
    if(isKeyWord("NIL")) {
        factor = creator.CreateConstNil();
        goto _end;
    }
    if(isKeyWord("TRUE")) {
        factor = creator.CreateConstBool(true);
        goto _end;
    }
    if(isKeyWord("FALSE")) {
        factor = creator.CreateConstBool(false);
        goto _end;
    }
    if(isString()) {
        factor = creator.CreateConstString(lexValue); //TODO check X
        goto _end;
    }
    if (isReal()) {
        float floatLexValue = std::stof(lexValue.c_str(), 0);
        factor = creator.CreateConstReal(floatLexValue);
        goto _end;
    }
    if(isInteger()) {
        int intLexValue;
        if (lexValue.back() == 'H') {
            intLexValue = std::stoll(lexValue.substr(0, lexValue.length() - 1).c_str(), 0, 16);
        }
        else {
            intLexValue = std::atoi(lexValue.c_str());
        }
        factor = creator.CreateConstInt(intLexValue);
        goto _end;
    }
    if(isConstSet()) {
        goto _end;
    }
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '~')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isDesignator()) {
        factor = curModule.GetConstFromName(lexValue);
        goto _4;
    }
    return { false, nullptr };
_1:
    std::pair<bool, ConstContext*> checkRes1 = isConstExpression();
    if(checkRes1.first) {
        factor = checkRes1.second;
        goto _2;
    }
    return { erMessage("Constant Expression expected"), nullptr };
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return { erMessage("Right bracket expected"), nullptr };
_3:
    std::pair<bool, ConstContext*> checkRes2 = isConstFactor();
    if(checkRes2.first) {
        factor = creator.CreateConstUnarExpr(checkRes2.second, "~");
        goto _end;
    }
    return { erMessage("Constant Factor expected"), nullptr };
_4:
    if(isActualParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return { true, factor };
}

//-----------------------------------------------------------------------------
// ConstSet = "{" [ConstElement {"," ConstElement}] "}".
bool ModuleCompiler::isConstSet() {
//_0:
   if(isSymbol(moduleStr[pos], '{')) {
       ++pos;
       ++column;
       ignore();
       goto _1;
   }
   return false;
_1:
   if(isSymbol(moduleStr[pos], '}')) {
       ++pos;
       ++column;
       ignore();
       goto _end;
   }
   if(isConstElement()) {
       goto _2;
   }
   return erMessage("Constant Element or '}' expected");
_2:
   if(isSymbol(moduleStr[pos], '}')) {
       ++pos;
       ++column;
       ignore();
       goto _end;
   }
   if(isSymbol(moduleStr[pos], ',')) {
       ++pos;
       ++column;
       ignore();
       goto _3;
   }
   return erMessage("'}' or ',' expected");
_3:
   if(isConstElement()) {
       goto _2;
   }
   return erMessage("Constant Element expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstElement = ConstExpression [".." ConstExpression].
bool ModuleCompiler::isConstElement() {
//_0:
    std::pair<bool, ConstContext*> checkRes1 = isConstExpression();
    if(checkRes1.first) {
        goto _1;
    }
    return false;
_1:
    if(moduleStr[pos]=='.' && moduleStr[pos+1]=='.') {
        pos += 2;
        column +=2;
        lexValue = "..";
        ignore();
        goto _2;
    }
    goto _end;
_2:
    std::pair<bool, ConstContext*> checkRes2 = isConstExpression();
    if(checkRes2.first) {
        goto _1;
    }
    return erMessage("Constant Expression expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// TypeDeclaration = identdef "=" type.
bool ModuleCompiler::isTypeDeclaration() {
//_0:
    std::string name;
    bool access = false;
    if(isIdentdef()) {
        name = lexValue;
        if (name.back() == '*') {
            name = name.substr(0, name.length() - 1);
            access = true;
        }
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '=')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    return erMessage("Symbol '=' expected");
_2:
    std::pair<bool, TypeContext*> typeRes = isType();
    if(typeRes.first) {
        curModule.AddNamedArtefact(name, typeRes.second, access);
        goto _end;
    }
    return erMessage("Type value expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// type = qualident | ArrayType | RecordType | PointerType | ProcedureType.
std::pair<bool, TypeContext*> ModuleCompiler::isType() {
//_0:
    TypeContext* resType;

    std::pair<bool, TypeContext*> arrayCheckRes = isArrayType();
    if(arrayCheckRes.first) {
        resType = arrayCheckRes.second;
        goto _end;
    }

    std::pair<bool, TypeContext*> recordCheckRes = isRecordType();
    if(recordCheckRes.first) {
        resType = recordCheckRes.second;
        goto _end;
    }

    std::pair<bool, TypeContext*> pointerCheckRes = isPointerType();
    if(pointerCheckRes.first) {
        resType = pointerCheckRes.second;
        goto _end;
    }

    std::pair<bool, TypeContext*> procCheckRes = isProcedureType();
    if (procCheckRes.first) {
        resType = procCheckRes.second;
        goto _end;
    }

    if(isQualident()) {
        resType = getTypeFromQualident(lexValue);
        if (resType == nullptr) {
            return { erMessage("Unknown type: " + lexValue), nullptr };
        }
        goto _end;
    }
    return { false, nullptr };
_end:
    return { true, resType };
}

TypeContext* ModuleCompiler::getTypeFromQualident(std::string qualident) {
    if (qualident == "INTEGER" || qualident == "BYTE") {
        return creator.CreateTypeInt();
    }
    if (qualident == "BOOLEAN") {
        return creator.CreateTypeBool();
    }
    if (qualident == "CHAR") {
        return creator.CreateTypeChar();
    }
    if (qualident == "REAL") {
        return creator.CreateTypeReal();
    }
    if (qualident == "SET") {
        return creator.CreateTypeSet();
    }
    return curModule.GetTypeFromName(qualident); //TODO проверить, не может ли быть Модуль.идент
}

//-----------------------------------------------------------------------------
// ArrayType = ARRAY length {"," length} OF type.
// length = ConstExpression.
std::pair<bool, TypeContext*> ModuleCompiler::isArrayType() {
//_0:
    if(isKeyWord("ARRAY")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    int size;
    std::pair<bool, ConstContext*> checkRes = isConstExpression();
    if(checkRes.first) {
        ConstContext* result = checkRes.second->getValue();
        if (result->getType() == "ERR") {
            return { erMessage("Error while constant evaluation: " + dynamic_cast<ConstErrContext*>(result)->getErrMsg()), nullptr };
        }
        if (result->getType() != "INT") {
            return { erMessage("Constant Expression of type INT expected, got "+ result->getType()), nullptr };
        }
        size = (static_cast<ConstIntContext*>(result))->getIntValue();
        goto _2;
    }
    return { erMessage("Constant Expression expected"), nullptr };
_2:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isKeyWord("OF")) {
        goto _3;
    }
    return { erMessage("Symbol ',' or Key word OF expected"), nullptr };
_3:
    std::pair<bool, TypeContext*> resType = isType();
    if (resType.first) {
        goto _end;
    }
    return { erMessage("Type value expected"), nullptr };
_end:
    return { true, creator.CreateTypeArray(resType.second, size)};
}

//-----------------------------------------------------------------------------
// RecordType = RECORD ["(" BaseType ")"] [FieldListSequence] END.
// BaseType = qualident.
// FieldListSequence = FieldList {";" FieldList}.
std::pair<bool, TypeContext*> ModuleCompiler::isRecordType() {
//_0:
    TypeRecordContext* record;
    TypeContext* parent;
    if(isKeyWord("RECORD")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    record = creator.CreateTypeRecord();
    if(isFieldList(record)) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return { erMessage("Symbol '(' or Key word END or Field list expected"), nullptr };
_2:
    if(isQualident()) {
        TypeContext* parent = getTypeFromQualident(lexValue);
        if (TypeRecordContext* recordType = dynamic_cast<TypeRecordContext*>(parent)) {
            record = creator.CreateTypeRecord(recordType);
            goto _3;
        }
        return { erMessage("Expected record type"), nullptr };
    }
    return { erMessage("Qualident expected"), nullptr };
_3:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _4;
    }
    return { erMessage("Symbol ')' expected"), nullptr };
_4:
    if(isFieldList(record)) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return { erMessage("Key word END or Field list expected"), nullptr };
_5:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return { erMessage("Key word END or Symbol ';' expected"), nullptr };
_6:
    if(isFieldList(record)) {
        goto _5;
    }
    return { erMessage("Field list expected"), nullptr };
_end:
    return { true, record };
}

//-----------------------------------------------------------------------------
// FieldList = IdentList ":" type.
// IdentList = identdef {"," identdef}.
bool ModuleCompiler::isFieldList(TypeRecordContext* record) {
//_0:
    std::vector<std::string> names;
    if(isIdentdef()) {
        names.push_back(lexValue);
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Symbols ',' or ':' expected");
_2:
    if(isIdentdef()) {
        names.push_back(lexValue);
        goto _1;
    }
    return erMessage("Idendef expected");
_3:
    std::pair<bool, TypeContext*> resType = isType();
    if (resType.first) {
        for (std::string name : names) {
            if (name.back() == '*') {
                name = name.substr(0, name.length() - 1);
                record->AddNamedModuleField(name, resType.second, true);
            }
            else {
                record->AddNamedModuleField(name, resType.second, false);
            }
        }
        goto _end;
    }
    return erMessage("Type expected");
_end:
    return true;
}


//-----------------------------------------------------------------------------
// PointerType = POINTER TO type.
std::pair<bool, TypeContext*> ModuleCompiler::isPointerType() {
//_0:
    if(isKeyWord("POINTER")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    if(isKeyWord("TO")) {
        goto _2;
    }
    return {erMessage("POINTER TO expected"), nullptr };
_2:
    TypeRecordContext* recordType;
    std::pair<bool, TypeContext*> resType = isType();
    if(resType.first) {
        if (recordType = dynamic_cast<TypeRecordContext*>(resType.second)) {
            goto _end;
        }
        return { erMessage("Expected record type"), nullptr };
    }
    return {erMessage("Type expected"), nullptr };
_end:
    return {true, creator.CreateTypePointer(recordType) };
}

//-----------------------------------------------------------------------------
// ProcedureType = PROCEDURE [FormalParameters].
std::pair<bool, ProcContext*> ModuleCompiler::isProcedureType() {
//_0:
    if(isKeyWord("PROCEDURE")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    ProcContext* proc = new ProcContext();
    if(isFormalParameters(proc)) {
        goto _end;
    }
    goto _end;
_end:
    return { true, proc };
}

//-----------------------------------------------------------------------------
// FormalParameters = "(" [FPSection {";" FPSection}] ")" [":" qualident].
bool ModuleCompiler::isFormalParameters(ProcContext* proc) {
//_0:
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isFPSection(proc)) {
        goto _2;
    }
    return erMessage("FPSection expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("')' or ';' expected");
_3:
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _4;
    }
    // Процедура без результата
    goto _end;
    ///return erMessage("':' expected");
_4:
    if(isQualident()) {
        TypeContext* returnType = getTypeFromQualident(lexValue);
        if (returnType != nullptr) {
            proc->setResultType(returnType);
            goto _end;
        }
        return erMessage("Unknown type: "+lexValue);
    }
    return erMessage("Qualident expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// FPSection = [VAR] ident {"," ident} ":" FormalType.
// FormalType = {ARRAY OF} qualident.
bool ModuleCompiler::isFPSection(ProcContext* proc) {
//_0:
    std::vector<std::string> names;
    if(isKeyWord("VAR")) {
        goto _1;
    }
    if(isIdent()) {
        names.push_back(lexValue);
        goto _2;
    }
    return false;
_1:
    if(isIdent()) {
        names.push_back(lexValue);
        goto _2;
    }
    // Вывод сообщения об ошибке
    return erMessage("Name of formal parameter expected");
_2:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("',' or ':' expected");
_3:
    if(isKeyWord("ARRAY")) {
        goto _4;
    }
    if(isQualident()) {
        TypeContext* argType = getTypeFromQualident(lexValue);
        if (argType != nullptr) {
            for (std::string name: names) {
                proc->AddNamedArtefact(name, argType);
            }
            //proc->setResultType(returnType);
            goto _end;
        }
        return erMessage("Unknown type: " + lexValue);
    }
    return erMessage("Key word ARRAY or Qualident expected");
_4:
    if(isKeyWord("OF")) {
        goto _5; //TODO по отчету об Обероне 7 кажется, что может быть много Array of подряд. Возможно поставить тут _3
    }
    return erMessage("Key word OF expected");
_5:
    if(isQualident()) {
        TypeContext* argType = getTypeFromQualident(lexValue);
        if (argType != nullptr) {
            TypeArrayContext* arrayArgType = creator.CreateTypeArray(argType, 0);
            for (std::string name : names) {
                proc->AddNamedArtefact(name, arrayArgType);
            }
            goto _end;
        }
        return erMessage("Unknown type: ARRAY OF " + lexValue);
    }
    return erMessage("Qualident expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// VariableDeclaration = IdentList ":" type.
// IdentList = identdef {"," identdef}.
bool ModuleCompiler::isVariableDeclaration() {
//_0:
    std::vector<std::string> names;
    if (isIdentdef()) {
        names.push_back(lexValue);
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Symbols ',' or ':' expected");
_2:
    if(isIdentdef()) {
        names.push_back(lexValue);
        goto _1;
    }
    return erMessage("Idendef expected");
_3:
    std::pair<bool, TypeContext*> resType = isType();
    if (resType.first) {
        for (std::string name : names) {
            if (name.back() == '*') {
                name = name.substr(0, name.length() - 1);
                curModule.AddNamedArtefact(name, creator.CreateVariable(name, resType.second), true);
            }
            else {
                curModule.AddNamedArtefact(name, creator.CreateVariable(name, resType.second), false);
            }
        }
        goto _end;
    }
    return erMessage("Type expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody ident.
bool ModuleCompiler::isProcedureDeclaration() {
//_0:
    std::pair<bool, ProcContext*> isProcHeadRes = isProcedureHeading();
    if(isProcHeadRes.first) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    return erMessage("Symbol ';' expected");
_2:
    if(isProcedureBody()) {
        goto _3;
    }
    return erMessage("Procedure body expected");
_3:
    if(isIdent()) {
        goto _end;
    }
    return erMessage("Name of Procedure expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureHeading = PROCEDURE identdef [FormalParameters].
std::pair<bool, ProcContext*> ModuleCompiler::isProcedureHeading() {
//_0:
    if(isKeyWord("PROCEDURE")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    ProcContext* proc = new ProcContext();
    if(isIdentdef()) {
        std::string name = lexValue;
        if (name.back() == '*') {
            name = name.substr(0, name.length() - 1);
            curModule.AddNamedArtefact(name, proc, true);
        }
        else {
            curModule.AddNamedArtefact(name, proc, false);
        }
        proc->setProcedureName(name);
        goto _2;
    }
    return { erMessage("Identdef expected"), nullptr };
_2:
    if(isFormalParameters(proc)) {
        goto _end;
    }
    goto _end;
_end:
    return { true, proc };
}

//-----------------------------------------------------------------------------
// ProcedureBody = DeclarationSequence [BEGIN StatementSequence]
//                 [RETURN expression] END.
bool ModuleCompiler::isProcedureBody() {
//_0:
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
    if(isDeclarationSequence()) {
        goto _1;
    }
    if(isKeyWord("BEGIN")) {
        goto _2;
    }
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return false;
_1:
    if(isKeyWord("BEGIN")) {
        goto _2;
    }
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("");
_2:
    stSeqCheckRes = isStatementSequence();
    if (stSeqCheckRes.first) {
        goto _3;
    }
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or RETURN or END expected");
_3:
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("RETURN or END expected");
_4:
    std::pair<bool, ExprContext*> checkRes = isExpression();
    if (checkRes.first) {
        goto _5;
    }
    return erMessage("Expression expected");
_5:
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("END expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// StatementSequence = statement {";" statement}.
std::pair<bool, std::vector<StatementContext*>> ModuleCompiler::isStatementSequence() {
//_0:
    std::vector<StatementContext*> res;
    std::pair<bool, StatementContext*> checkRes1 = isStatement();
    if (checkRes1.first) {
        res.push_back(checkRes1.second);
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return { false, res };
_1:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    goto _end;
_2:
    std::pair<bool, StatementContext*> checkRes2 = isStatement();
    if (checkRes2.first) {
        res.push_back(checkRes2.second);
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    goto _end;
_end:
    return { true, res };
}

//-----------------------------------------------------------------------------
// statement = [assignment | ProcedureCall | IfStatement | CaseStatement |
//             WhileStatement | RepeatStatement | ForStatement].
std::pair<bool, StatementContext*> ModuleCompiler::isStatement() {
    std::pair<bool, StatementContext*> statementCheck = isIfStatement();
    if(statementCheck.first) {
        return { true, statementCheck.second };
    }
    statementCheck = isCaseStatement();
    if(statementCheck.first) {
        return { true, statementCheck.second };
    }
    statementCheck = isWhileStatement();
    if(statementCheck.first) {
        return { true, statementCheck.second };
    }
    statementCheck = isRepeatStatement();
    if(statementCheck.first) {
        return { true, statementCheck.second };
    }
    statementCheck = isForStatement();
    if(statementCheck.first) {
        return { true, statementCheck.second };
    }
    ///if(isAssignmentOrProcedureCall()) {
    ///    return true;
    ///}
    statementCheck = isAssignment();
    if(statementCheck.first) {
        return { true, statementCheck.second };
    }
    if(isProcedureCall()) {
        return { true, nullptr };
    }
    return { false, nullptr };
}

//-----------------------------------------------------------------------------
// assignment = designator ":=" expression.
std::pair<bool, StatementContext*> ModuleCompiler::isAssignment() {
    Location l;
    storeLocation(l);
    /// Тестовая точка входа в правило
    ///std::cout << "----> isAssignment" << std::endl;
//_0:
    std::string varName;
    if(isDesignator()) {
        varName = lexValue;
        goto _1;
    }
    restoreLocation(l);
    return { false, nullptr };
_1:
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        goto _2;
    }
    // Откат на анализ вызова процедуры
    restoreLocation(l);
    return { false, nullptr };
    // return erMessage("':=' expected");
_2:
    VarContext* var;
    var = curModule.GetVarFromName(varName);
    if (var == nullptr) {
        return { erMessage("Incorrect variable name: "+varName) , nullptr};
    }
    std::pair<bool, ExprContext*> checkRes = isExpression();
    if (checkRes.first) {
        if (var->getType()->getTypeName() != checkRes.second->getResType()) {
            return { erMessage("Incorrect variable assignment: Can't assign " + checkRes.second->getResType() + " to " + var->getType()->getTypeName()) , nullptr};
        }
        goto _end;
    }
    return { erMessage("Expression expected"), nullptr };
_end:
    /// Тестовая точка выхода из правила
    ///std::cout << "isAssignmentOrProcedure ---->" << std::endl;
    return { true, creator.CreateAssignmentStatement(var, checkRes.second)};
}

//-----------------------------------------------------------------------------
// ProcedureCall = designator [ActualParameters].
bool ModuleCompiler::isProcedureCall() {
//_0:
    if(isDesignator()) {
        goto _1;
    }
    return false;
_1:
    if(isActualParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// IfStatement = IF expression THEN StatementSequence
//              {ELSIF expression THEN StatementSequence}
//              [ELSE StatementSequence] END.
std::pair<bool, StatementContext*> ModuleCompiler::isIfStatement() {
//_0:isAssignmentOrProcedure
    std::pair<bool, ExprContext*> exprCheckRes;
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
    std::vector<ConditionalStatementsBlock> blocks;
    if(isKeyWord("IF")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    exprCheckRes = isExpression();
    if (exprCheckRes.first) {
        goto _2;
    }
    return {erMessage("Expression expected"), nullptr };
_2:
    if(isKeyWord("THEN")) {
        goto _3;
    }
    return {erMessage("Key word THEN expected"), nullptr };
_3:
    stSeqCheckRes = isStatementSequence();
    if(stSeqCheckRes.first) {
        ConditionalStatementsBlock block;
        block.setCondition(exprCheckRes.second);
        block.setStatements(stSeqCheckRes.second);
        blocks.push_back(block);
        goto _4;
    }
    if(isKeyWord("ELSIF")) {
        goto _1;
    }
    if(isKeyWord("ELSE")) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return {erMessage("Statement Sequence or ELSIF or ELSE or END expected"), nullptr };
_4:
    if(isKeyWord("ELSIF")) {
        goto _1;
    }
    if(isKeyWord("ELSE")) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return {erMessage("ELSIF or ELSE or END expected"), nullptr };
_5:
    stSeqCheckRes = isStatementSequence();
    if (stSeqCheckRes.first) {
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return {erMessage("Statement Sequence or END expected"), nullptr };
_6:
    if(isKeyWord("END")) {
        ConditionalStatementsBlock block;
        block.setStatements(stSeqCheckRes.second);
        blocks.push_back(block);
        goto _end;
    }
    return {erMessage("END expected"), nullptr };
_end:
    return { true , creator.CreateIfStatement(blocks)};
}

//-----------------------------------------------------------------------------
// CaseStatement = CASE expression OF case {"|" case} END.
// case = [CaseLabelList ":" StatementSequence].
// CaseLabelList = LabelRange {"," LabelRange}.
// LabelRange = label [".." label].
// label = integer | string | qualident.
std::pair<bool, StatementContext*> ModuleCompiler::isCaseStatement() {
//_0:
    std::vector<ConditionalStatementsBlock> blocks;
    ExprContext* caseCond = nullptr;
    ExprContext* temp = nullptr;
    std::pair<bool, ExprContext*> expr1CheckRes;
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
    if(isKeyWord("CASE")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    expr1CheckRes = isExpression();
    if (expr1CheckRes.first) {
        goto _2;
    }
    return {erMessage("Expression expected"), nullptr };
_2:
    if(isKeyWord("OF")) {
        goto _3;
    }
    return {erMessage("Key word OF expected"), nullptr };
_3:
    temp = nullptr;
    if(isInteger()) {
        int intLexValue;
        if (lexValue.back() == 'H') {
            intLexValue = std::stoll(lexValue.substr(0, lexValue.length() - 1).c_str(), 0, 16);
        }
        else {
            intLexValue = std::atoi(lexValue.c_str());
        }
        temp = creator.CreateIntExpr(intLexValue);
        goto _4;
    }
    if(isString()) {
        temp = creator.CreateStringExpr(lexValue);
        goto _4;
    }
    if(isQualident()) {
        if (curModule.GetConstFromName(lexValue) != nullptr) {
            temp = creator.CreateConstValueExpr(curModule.GetConstFromName(lexValue));
        }
        if (curModule.GetVarFromName(lexValue) != nullptr) {
            temp = creator.CreateVarValueExpr(curModule.GetVarFromName(lexValue));
        }
        if (temp == nullptr) {
            return {erMessage("Unknown qualident " + lexValue), nullptr };
        }
        goto _4;
    }
    return {erMessage("Integer or String or Qualified Ident expected"), nullptr };
_4:
    if (caseCond == nullptr) {
        caseCond = creator.CreateExpr(expr1CheckRes.second, temp, "=");
    }
    else {
        caseCond = creator.CreateExpr(caseCond, creator.CreateExpr(expr1CheckRes.second, temp, "="), "OR");
    }
    if(moduleStr[pos]=='.' && moduleStr[pos+1]=='.') {
        pos += 2;
        column += 2;
        lexValue = "..";
        ignore();
        goto _5;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _7;
    }
    return {erMessage("'..' or ',' or ':' expected"), nullptr };
_5:
    if(isInteger()) {
        goto _6;
    }
    if(isString()) {
        goto _6;
    }
    if(isQualident()) {
        goto _6;
    }
    return {erMessage("Integer or String or Qualified Ident expected"), nullptr };
_6:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _7;
    }
    return {erMessage("',' or ':' expected"), nullptr };
_7:
    stSeqCheckRes = isStatementSequence();
    if (stSeqCheckRes.first) {
        ConditionalStatementsBlock block;
        block.setCondition(caseCond);
        block.setStatements(stSeqCheckRes.second);
        blocks.push_back(block);
        goto _8;
    }
    if(isSymbol(moduleStr[pos], '|')) {
        caseCond = nullptr;
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return {erMessage("Statement Sequence or '|' or END expected"), nullptr };
_8:
    if(isSymbol(moduleStr[pos], '|')) {
        caseCond = nullptr;
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return {erMessage("'|' or END expected"), nullptr };
_end:
    return { true, creator.CreateCaseStatement(blocks) };
}

//-----------------------------------------------------------------------------
// WhileStatement = WHILE expression DO StatementSequence
//                 {ELSIF expression DO StatementSequence} END.
std::pair<bool, StatementContext*> ModuleCompiler::isWhileStatement() {
    std::pair<bool, ExprContext*> exprCheckRes;
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
    std::vector<WhileStatementsBlock> blocks;
    //_0:
        if(isKeyWord("WHILE")) {
            goto _1;
        }
        return { false, nullptr };
    _1:
        exprCheckRes = isExpression();
        if (exprCheckRes.first) {
            goto _2;
        }
        return { erMessage("Expression expected"), nullptr };
    _2:
        if(isKeyWord("DO")) {
            goto _3;
        }
        return { erMessage("Key word DO expected"), nullptr };
    _3:
        stSeqCheckRes = isStatementSequence();
        if (stSeqCheckRes.first) {
            WhileStatementsBlock block;
            block.setCondition(exprCheckRes.second);
            block.setStatements(stSeqCheckRes.second);
            blocks.push_back(block);
            goto _4;
        }
        if(isKeyWord("ELSIF")) {
            goto _1;
        }
        if(isKeyWord("END")) {
            goto _end;
        }
        return { erMessage("Statement Sequence or ELSIF or END expected"), nullptr };
    _4:
        if(isKeyWord("ELSIF")) {
            goto _1;
        }
        if(isKeyWord("END")) {
            goto _end;
        }
        return { erMessage("ELSIF or END expected"), nullptr };
    _end:
        return { true, creator.CreateWhileStatement(blocks) };
}

//-----------------------------------------------------------------------------
// RepeatStatement = REPEAT StatementSequence UNTIL expression.
std::pair<bool, StatementContext*> ModuleCompiler::isRepeatStatement() {
    std::pair<bool, ExprContext*> exprCheckRes;
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
//_0:
    if(isKeyWord("REPEAT")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    stSeqCheckRes = isStatementSequence();
    if (stSeqCheckRes.first) {
        goto _2;
    }
    if(isKeyWord("UNTIL")) {
        goto _3;
    }
    return { erMessage("Statement Sequence or UNTIL expected"), nullptr };
_2:
    if(isKeyWord("UNTIL")) {
        goto _3;
    }
    return { erMessage("Key word UNTIL expected"), nullptr };
_3:
    exprCheckRes = isExpression();
    if (exprCheckRes.first) {
        goto _end;
    }
    return { erMessage("Expression expected"), nullptr };
_end:
    return { true, creator.CreateRepeatStatement(exprCheckRes.second, stSeqCheckRes.second) };
}

//-----------------------------------------------------------------------------
// ForStatement = FOR ident ":=" expression TO expression [BY ConstExpression]
//                DO StatementSequence END.
std::pair<bool, StatementContext*> ModuleCompiler::isForStatement() {
    VarContext* var = nullptr;
    StatementContext* init = nullptr;
    ExprContext* cond = nullptr;
    int stepVal = 1;
    StatementContext* step = nullptr;
    std::pair<bool, ExprContext*> exprCheckRes;
    std::pair<bool, ConstContext*> constCheckRes;
    std::pair<bool, std::vector<StatementContext*>> stSeqCheckRes;
//_0:
    if(isKeyWord("FOR")) {
        goto _1;
    }
    return { false, nullptr };
_1:
    if((isIdent())) {
        if (curModule.GetVarFromName(lexValue) != nullptr) {
            var = curModule.GetVarFromName(lexValue);
        }
        else {
            return { erMessage("Unknown variable "+lexValue), nullptr };
        }
        goto _2;
    }
    return { erMessage("Identifier expected"), nullptr };
_2:
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        goto _3;
    }
    return { erMessage("':=' expected"), nullptr };
_3:
    exprCheckRes = isExpression();
    if (exprCheckRes.first) {
        init = creator.CreateAssignmentStatement(var, exprCheckRes.second);
        goto _4;
    }
    return { erMessage("Expression expected"), nullptr };
_4:
    if(isKeyWord("TO")) {
        goto _5;
    }
    return { erMessage("Key word TO expected"), nullptr };
_5:
    exprCheckRes = isExpression();
    if(exprCheckRes.first) {
        cond = exprCheckRes.second;
        goto _6;
    }
    return { erMessage("Expression expected"), nullptr };
_6:
    if(isKeyWord("BY")) {
        goto _7;
    }
    if(isKeyWord("DO")) {
        goto _9;
    }
    return { erMessage("Key word BY or DO expected"), nullptr };
_7:
    constCheckRes = isConstExpression();
    if(constCheckRes.first) {
        if (constCheckRes.second->getValue()->getType() != "INT") {
            return { erMessage("FOR step must be integer"), nullptr };
        }
        stepVal = (static_cast<ConstIntContext*>(constCheckRes.second->getValue()))->getIntValue();
        goto _8;
    }
    return { erMessage("Constant Expression expected"), nullptr };
_8:
    if(isKeyWord("DO")) {
        goto _9;
    }
    return { erMessage("Key word DO expected"), nullptr };
_9:
    stSeqCheckRes = isStatementSequence();
    if (stSeqCheckRes.first) {
        goto _10;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return { erMessage("Statement Sequence or END expected"), nullptr };
_10:
    if(isKeyWord("END")) {
        goto _end;
    }
    return { erMessage("END expected"), nullptr };
_end:
    step = creator.CreateAssignmentStatement(var, creator.CreateExpr(creator.CreateVarValueExpr(var), creator.CreateIntExpr(stepVal), "+"));
    if (stepVal > 0) {
        cond = creator.CreateExpr(creator.CreateVarValueExpr(var), cond, "<");
    }
    else {
        cond = creator.CreateExpr(creator.CreateVarValueExpr(var), cond, ">");
    }
    return { true, creator.CreateForStatement(init, cond, step, stSeqCheckRes.second) };
}


//-----------------------------------------------------------------------------
// expression = SimpleExpression [relation SimpleExpression].
std::pair<bool, ExprContext*> ModuleCompiler::isExpression() {
//_0:
    std::string operName;
    std::pair<bool, ExprContext*> checkRes1 = isSimpleExpression();
    ExprContext* firstOperand;
    if (checkRes1.first) {
        firstOperand = checkRes1.second;
        goto _1;
    }
    return { false, nullptr };
_1:
    if(isRelation()) {
        operName = lexValue;
        goto _2;
    }
    goto _end;
_2:
    std::pair<bool, ExprContext*> checkRes2 = isSimpleExpression();
    if (checkRes2.first) {
        firstOperand = creator.CreateExpr(firstOperand, checkRes2.second, operName);
        goto _end;
    }
    return { erMessage("Simple Expression expected"), nullptr };
_end:
    return { true, firstOperand };
}

//-----------------------------------------------------------------------------
// SimpleExpression = ["+" | "-"] term {AddOperator term}.
std::pair<bool, ExprContext*> ModuleCompiler::isSimpleExpression() {
//_0:
    std::string unarOpName = "";
    std::string operName;
    ExprContext* firstOperand = nullptr;
    std::string tmpValue = "";
    if(isSymbol(moduleStr[pos], '+')) {
        ++pos;
        ++column;
        ignore();
        unarOpName = "+";
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '-')) {
        ++pos;
        ++column;
        ignore();
        unarOpName = "-";
        goto _1;
    }
    std::pair<bool, ExprContext*> checkRes1 = isTerm();
    if (checkRes1.first) {
        firstOperand = checkRes1.second;
        goto _2;
    }
    return { false, nullptr };
_1:
    std::pair<bool, ExprContext*> checkRes2 = isTerm();
    if (checkRes2.first) {
        if (firstOperand != nullptr) {
            firstOperand = creator.CreateExpr(firstOperand, checkRes2.second, operName);
        }
        else {
            firstOperand = creator.CreateUnarExpr(checkRes2.second, unarOpName);
        }
        goto _2;
    }
    return { erMessage("Term expected"), nullptr };
_2:
    if(isAddOperator()) {
        operName = lexValue;
        goto _1;
    }
    goto _end;
_end:
    return { true, firstOperand };
}

//-----------------------------------------------------------------------------
// term = factor {MulOperator factor}.
std::pair<bool, ExprContext*> ModuleCompiler::isTerm() {
//_0:
    std::pair<bool, ExprContext*> checkRes1 = isFactor();
    std::string operName;
    ExprContext* firstOperand;
    if (checkRes1.first) {
        firstOperand = checkRes1.second;
        goto _1;
    }
    return { false, nullptr };
_1:
    if(isMulOperator()) {
        operName = lexValue;
        goto _2;
    }
    goto _end;
_2:
    std::pair<bool, ExprContext*> checkRes2 = isFactor();
    if (checkRes2.first) {
        firstOperand = creator.CreateExpr(firstOperand, checkRes2.second, operName);
        goto _1;
    }
    return { erMessage("Factor expected"), nullptr };
_end:
    return { true, firstOperand };
}

//-----------------------------------------------------------------------------
// factor = number | string | NIL | TRUE | FALSE |
//       set | designator [ActualParameters] | "(" expression ")" | "~" factor.
// number = integer | real
std::pair<bool, ExprContext*> ModuleCompiler::isFactor() {
//_0:
    ExprContext* factor = nullptr;
    if(isKeyWord("NIL")) {
        factor = creator.CreateNilExpr();
        goto _end;
    }
    if(isKeyWord("TRUE")) {
        factor = creator.CreateBoolExpr(true);
        goto _end;
    }
    if(isKeyWord("FALSE")) {
        factor = creator.CreateBoolExpr(false);
        goto _end;
    }
    if(isString()) {
        factor = creator.CreateStringExpr(lexValue);
        goto _end;
    }
    if (isReal()) {
        float floatLexValue = std::stof(lexValue.c_str(), 0);
        factor = creator.CreateRealExpr(floatLexValue);
        goto _end;
    }
    if (isInteger()) {
        int intLexValue;
        if (lexValue.back() == 'H') {
            intLexValue = std::stoll(lexValue.substr(0, lexValue.length() - 1).c_str(), 0, 16);
        }
        else {
            intLexValue = std::atoi(lexValue.c_str());
        }
        factor = creator.CreateIntExpr(intLexValue);
        goto _end;
    }
    if(isSet()) {
        goto _end;
    }
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '~')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isDesignator()) {    
        if (curModule.GetConstFromName(lexValue) != nullptr) {
            factor = creator.CreateConstValueExpr(curModule.GetConstFromName(lexValue));
        }
        if (curModule.GetVarFromName(lexValue) != nullptr) {
            factor = creator.CreateVarValueExpr(curModule.GetVarFromName(lexValue));
        }
        if (factor == nullptr) {
            return { erMessage("Unknown designator " + lexValue), nullptr };
        }
        goto _4;
    }
    return { false,nullptr };
_1:
    std::pair<bool, ExprContext*> checkRes1 = isExpression();
    if (checkRes1.first) {
        factor = checkRes1.second;
        goto _2;
    }
    return { erMessage("Expression expected"), nullptr };
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return { erMessage("Right bracket expected"),nullptr };
_3:
    std::pair<bool, ExprContext*> checkRes2 = isFactor();
    if (checkRes2.first) {
        goto _end;
    }
    return { erMessage("Factor expected"), nullptr };
_4:
    if(isActualParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return { true, factor };
}

//-----------------------------------------------------------------------------
// designator = qualident {selector}.
// selector = "." ident | "[" ExpList "]" | "^" | "(" qualident ")".
bool ModuleCompiler::isDesignator() {
    Location l;
//_0:
    if(isQualident()) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '.')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isSymbol(moduleStr[pos], '[')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], '^')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '(')) {
        // Фиксируем позицию скобки для возможного отката назад в случае,
        // когда обозначение не содержит круглые скобки,
        // а является вызовом процедуры.
        // В дальнейшем более точный анализ буде возлагаться на
        // семантический разбор
        storeLocation(l);
        ++pos;
        ++column;
        ignore();
        goto _5;
    }
    goto _end;
_2:
    if(isIdent()) {
        goto _1;
    }
    return erMessage("'.' expected");
_3:
    std::pair<bool, ExprContext*> checkRes = isExpression();
    if (checkRes.first) {
        goto _4;
    }
    return erMessage("ExpList expected");
_4:
    if(isSymbol(moduleStr[pos], ']')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("']' or ',' expected");
_5:
    if(isQualident()) {
        goto _6;
    }
    // Возможна процедура без параметров. Поэтому откат на анализ
    // фактических параметров
    restoreLocation(l);
    goto _end;
    ///return erMessage("Qualident expected");
_6:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    // Здесь возможен возврат обозначения без круглых скобок,
    // котрые могут принадлежать списку фактических параметров.
    // Поэтому ситуации, когда не закрывающаяся круглая скобка,
    // откатываемся назад.
    // Хотя возможно, что под обозначение может попасть и процедура
    // с одним фактическим параметром.
    // Но в этой ситуации дальше будет разбираться семантический анализ.
    restoreLocation(l);
    goto _end;
    ///return erMessage("')' expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// set = "{" [element {"," element}] "}".
// element = expression [".." expression].
bool ModuleCompiler::isSet() {
//_0:
    if(isSymbol(moduleStr[pos], '{')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    std::pair<bool, ExprContext*> checkRes1 = isExpression();
    if (checkRes1.first) {
        goto _2;
    }
    return erMessage("'}' or Expression expected");
_2:
    if(moduleStr[pos]=='.' && moduleStr[pos+1]=='.') {
        pos += 2;
        column += 2;
        lexValue = "..";
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("'..'or '}' or ',' expected");
_3:
    std::pair<bool, ExprContext*> checkRes2 = isExpression();
    if (checkRes2.first) {
        goto _4;
    }
    return erMessage("Expression expected");
_4:
    if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("'}' or ',' expected");
_end:
    return true;
}

/**
//-----------------------------------------------------------------------------
// ExpList = expression {"," expression}.
bool ModuleCompiler::isExpList() {
//_0:
    return false;
_end:
    return true;
}
*/

//-----------------------------------------------------------------------------
// ActualParameters = "(" [ExpList] ")" .
// ExpList = expression {"," expression}.
bool ModuleCompiler::isActualParameters() {
    /// Тестовая точка входа в правило
    ///std::cout << "----> isActualParameters" << std::endl;
//_0:
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    std::pair<bool, ExprContext*> checkRes = isExpression();
    if (checkRes.first) {
        goto _2;
    }
    return erMessage("')' or Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("')' or ',' expected");
_end:
    /// Тестовая точка входа в правило
    ///std::cout << "isActualParameters ---->OK" << std::endl;
    return true;
}

//-----------------------------------------------------------------------------
// Правила, определяющие лексический анализ
//-----------------------------------------------------------------------------

// Идентификатор без игнорируемых символов - основа всех других буквенно-цифровых
// конструкций
// Id = Letter {Letter | Digit}.
bool ModuleCompiler::isId() {
    Location l;
    storeLocation(l);
    std::string tmpValue = "";
    if(isLetter(moduleStr[l.pos])) {
        tmpValue += moduleStr[l.pos];
        ++l.pos;
        ++l.column;
        while(isLetterOrDigit(moduleStr[l.pos])) {
            tmpValue += moduleStr[l.pos];
            ++l.pos;
            ++l.column;
        }
        if(!isKeyWordValue(tmpValue)) {
            restoreLocation(l);
            lexValue = tmpValue;
            /// Тестовый вывод (закомментировать после отработки)
            ///testMessage("It's Identdef = " + lexValue);
            return true;
        }
    }
    return false;
}


// Ident = Letter {Letter | Digit}.
bool ModuleCompiler::isIdent() {
    if(isId()) {
        /// Тестовый вывод (закомментировать после отработки)
        testMessage("It's Identdef = " + lexValue);
        ignore();
        return true;
    }
    return false;
}

// identdef = ident ["*"].
bool ModuleCompiler::isIdentdef() {
    std::string tmpValue = "";
    if(isId()) {
        // Возможна звездочка
        if(isSymbol(moduleStr[pos], '*')) {
            ++pos;
            ++column;
            lexValue += '*';
        }
        /// Тестовый вывод (закомментировать после отработки)
        testMessage("It's Identdef = " + lexValue);
        ignore();
        return true;
    }
    return false;
}

// qualident = [ident "."] ident.
bool ModuleCompiler::isQualident() {
    Location l;
    std::string tmpValue = "";
    // Проверка на первый идентификатор
    if(isId()) {
        tmpValue = lexValue;
        goto _1;
    }
    return false;
_1:
    // При наличии первого идентификатора возможна вторая часть
    //или корректный выход
    if(isSymbol(moduleStr[pos], '.')) {
        storeLocation(l);   // Возможно, что точка от перечисления диапазона
        ++pos;
        ++column;
        goto _2;
    }
    goto _end;
_2:
    // Проверка на второй идентификатор
    if(isId()) {
        tmpValue += '.' + lexValue;
        goto _end;
    }
    // Если не второй идентификатор, то возможен qualid из одного перед перечислением
    // Поэтому возвращается накопленное значение с восстановлением позиции
    restoreLocation(l);
    goto _end;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Qualident = " + lexValue);
    ignore();
    return true;
}

// KeyWord.
bool ModuleCompiler::isKeyWord(const std::string&& str) {
    Location l;
    storeLocation(l);
    std::string tmpValue = "";
    if(isLetter(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        while(isLetterOrDigit(moduleStr[pos])) {
            tmpValue += moduleStr[pos];
            ++pos;
            ++column;
        }
        if(tmpValue==str) {
            lexValue = tmpValue;
            /// Тестовый вывод (закомментировать после отработки)
            testMessage("It's Key word = " + lexValue);
            ignore();
            return true;
        }
    }
    // Откат, когда не то слово
    restoreLocation(l);
    return false;
}

// AssignSymbol = ":="
bool ModuleCompiler::isAssignSymbol() {
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// relation = "=" | "#" | "<" | "<=" | ">" | ">=" | IN | IS.
bool ModuleCompiler::isRelation() {
    if(moduleStr[pos]=='=') {
        ++pos;
        ++column;
        lexValue = "=";
        ignore();
        return true;
    } else if(moduleStr[pos]=='#') {
        ++pos;
        ++column;
        lexValue = "#";
        ignore();
        return true;
    } else if(moduleStr[pos]=='<' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = "<=";
        ignore();
        return true;
    } else if(moduleStr[pos]=='<') {
        ++pos;
        ++column;
        lexValue = "<";
        ignore();
        return true;
    } else if(moduleStr[pos]=='>' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ">=";
        ignore();
        return true;
    } else if(moduleStr[pos]=='>') {
        ++pos;
        ++column;
        lexValue = ">";
        ignore();
        return true;
    } else if(isKeyWord("IN")) {
        return true;
    } else if(isKeyWord("IS")) {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// AddOperator = "+" | "-" | OR.
bool ModuleCompiler::isAddOperator() {
    if(moduleStr[pos]=='+') {
        ++pos;
        ++column;
        lexValue = "+";
        ignore();
        return true;
    } else if(moduleStr[pos]=='-') {
        ++pos;
        ++column;
        lexValue = "-";
        ignore();
        return true;
    } else if(isKeyWord("OR")) {
        ignore();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// MulOperator = "*" | "/" | DIV | MOD | "&".
bool ModuleCompiler::isMulOperator() {
    if(moduleStr[pos]=='*') {
        ++pos;
        ++column;
        lexValue = "*";
        ignore();
        return true;
    } else if(moduleStr[pos]=='/') {
        ++pos;
        ++column;
        lexValue = "/";
        ignore();
        return true;
    } else if(moduleStr[pos]=='&') {
        ++pos;
        ++column;
        lexValue = "&";
        ignore();
        return true;
    } else if(isKeyWord("DIV")) {
        ignore();
        return true;
    } else if(isKeyWord("MOD")) {
        ignore();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// number = integer | real.
bool ModuleCompiler::isNumber() {
    if(isReal()) {
        return true;
    }
    if(isInteger()) {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// digit {digit} "." {digit} [ScaleFactor].
// ScaleFactor = "E" ["+" | "-"] digit {digit}.
bool ModuleCompiler::isReal() {
//_0:
    Location l;
    storeLocation(l);
    std::string tmpValue = "";
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    return false;
_1:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(moduleStr[pos]=='.') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    restoreLocation(l);
    return false;
_2:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    if(moduleStr[pos]=='E') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _3;
    };
    goto _end;
_3:
    if((moduleStr[pos]=='+') || (moduleStr[pos]=='-')) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _4;
    };
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _5;
    }
    restoreLocation(l);
    return false;
_4:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _5;
    }
    restoreLocation(l);
    return false;
_5:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _5;
    }
    goto _end;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Real = " + lexValue);
    ignore();
    return true;
}

//-----------------------------------------------------------------------------
// digit {digit} | digit {hexDigit} "H".
bool ModuleCompiler::isInteger() {
//_0:
    ///storeLocation(l);
    std::string tmpValue = "";
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    return false;
_1:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(moduleStr[pos]=='H') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _end;
    }
    if((moduleStr[pos]>='A') && (moduleStr[pos]<='F')) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
_2:
    if(isHexDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(moduleStr[pos]=='H') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _end;
    }
_end:
    /// Возможно, где-то здесь нужно добавить дополнительную проверку, что после цифр
    /// сразу не идут буквы. То есть, что нет слияния числа с идентификатором.
    /// И в этом случае сделать откат.
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Integer = " + lexValue);
    ignore();
    return true;
}

// string = """ {character} """ | digit {hexDigit} "X".
bool ModuleCompiler::isString() {
    Location l;
    storeLocation(l);
//_0:
    std::string tmpValue = "";
    if(moduleStr[pos]=='"') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    return false;
_1:
    while(moduleStr[pos]!='"') {
        if(isEndOfString()) {
            return false;
        }
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
    }
    tmpValue += moduleStr[pos];
    ++pos;
    ++column;
    goto _end;
_2:
    if(((moduleStr[pos]>='A') && (moduleStr[pos]<='F'))
            || isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    if(moduleStr[pos]=='X') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _end;
    }
    // Откат назад, если не набраны символьные данные, для проверки чисел
    restoreLocation(l);
    return false;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's String = " + lexValue);
    ignore();
    return true;
}

// comment = "(*" {character} "*)".
bool ModuleCompiler::isComment() {
//_0:
    std::string tmpValue = "";
    if((moduleStr[pos]=='(') && (moduleStr[pos+1]=='*')) {
        pos += 2;
        column += 2;
        tmpValue = "(*";
        goto _1;
    }
    return false;
_1:
    if(moduleStr[pos]=='*' && moduleStr[pos+1]==')') {
        // Конец текущего комментария
        pos += 2;
        column += 2;
        tmpValue += "*)";
        goto _end;
    }
    if(isSymbol(moduleStr[pos], '\n')) {
        // Символ перевода строки внутри комментария
        // Замена в лексеме на обратный слэш
        tmpValue += "\\";
        ++pos;
        ++line;
        column = 1;
        goto _1;
    }
    if(isComment()) {
        // рекурсивное обращение к вложенному комментарию
        // Накопленный подкомментарий добавляется к текущему
        tmpValue += lexValue;
        goto _1;
    }
    if(isEndOfString()) {
        // Данный комментарий не закрыт, а строка закончилась.
        // В принципе с точки зрения языка это не ошибка.
        // Но лучше фиксировать эту ситуацию
        return erMessage("Comment = " + tmpValue + " isn't closed");
    }
    // Продолжение комментария при любых других ситуациях
    tmpValue += moduleStr[pos];
    ++pos;
    ++column;
    goto _1;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Comment = " + lexValue);
    //ignore();
    return true;
}

//-----------------------------------------------------------------------------
// Правила, определяющие транслитерацию
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Вспомогательные утилиты
//-----------------------------------------------------------------------------

// Пропуск пробельных символов и комментариев
// Ignore = {/ '\s' | '\n' | '\t' | Comment }
void ModuleCompiler::ignore() {
_0:
    if(isSymbol(moduleStr[pos], ' ')
            || isSymbol(moduleStr[pos], '\r')
            || isSymbol(moduleStr[pos], '\t'))
    {
        ++pos;
        ++column;
        goto _0;
    }
    if(isSymbol(moduleStr[pos], '\n')) {
        ++pos;
        ++line;
        column = 1;
        goto _0;
    }
    if(isComment()) {
        goto _0;
    }
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's ignored. Next symbol = " + std::string{moduleStr[pos]});
}

// Проверка на принадлежность идентификатора одному из
// множества ключевых или зарезервированных слов
bool ModuleCompiler::isKeyWordValue(std::string& ident) {
    for(std::string iStr: keyWords) {
        if(ident == iStr) {
            return true;
        }
    }
    return false;
}

// Проверка на окончание разбираемой строки
bool ModuleCompiler::isEndOfString() {
    ///return pos >= int(moduleStr.length());
    return moduleStr[pos] == '\0';
}

// Выдача сообщения об ошибке
bool ModuleCompiler::erMessage(std::string&& str) {
    // Вывод сообщения об ошибке
    std::cout << "ERROR ("
              << line << ", "
              << column << " {"
              << pos << "}): ";
    std::cout << str << std::endl;
    return false;
}

// Формирование типового тестового сообщения
void ModuleCompiler::testMessage(std::string str) {
    // Вывод сообщения об ошибке
    std::cout << "TEST ("
              << line << ", "
              << column << " {"
              << pos << "}): ";
    std::cout << str << std::endl;
}
