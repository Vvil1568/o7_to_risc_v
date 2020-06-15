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
}

// Конструктор, формирующий начальные установки параметров компилятора
ModuleCompiler::ModuleCompiler(const char* str): moduleStr{str},
    pos{0}, line{1}, column{1}
{}

//-----------------------------------------------------------------------------
// Module = MODULE ident ";" [ImportList] DeclarationSequence
//          [BEGIN StatementSequence] END ident "." .
bool ModuleCompiler::isModule() {
//_0:
    ignore();
    if(isKeyWord("MODULE")) {
        ignore();
        goto _1;
    }
    return erMessage("The Module must start using key word MODULE");
_1:
    if(isIdent()) {
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
    if(isStatementSequence()) {
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
//_0:
    if(isKeyWord("IMPORT")) {
        goto _1;
    }
    return false;
_1:
    if(isIdent()) {
        goto _2;
    }
    return erMessage("It's not the imported name or alias");
_2:
    if(isSymbol(moduleStr[pos], ',')) {
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
        goto _4;
    }
    return erMessage("It's not the imported name");
_4:
    if(isSymbol(moduleStr[pos], ',')) {
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
    if(isIdentdef()) {
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
    if(isConstExpression()) {
        goto _end;
    }
    return erMessage("Constant Expression expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstExpression = SimpleConstExpression [relation SimpleConstExpression].
bool ModuleCompiler::isConstExpression() {
//_0:
    if(isSimpleConstExpression()) {
        goto _1;
    }
    return false;
_1:
    if(isRelation()) {
        goto _2;
    }
    goto _end;
_2:
    if(isSimpleConstExpression()) {
        goto _end;
    }
    return erMessage("Simple Constant Expression expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// SimpleConstExpression = ["+" | "-"] ConstTerm {AddOperator ConstTerm}.
bool ModuleCompiler::isSimpleConstExpression() {
//_0:
    if(isSymbol(moduleStr[pos], '+')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '-')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isConstTerm()) {
        goto _2;
    }
    return false;
_1:
    if(isConstTerm()) {
        goto _2;
    }
    return erMessage("Constant Term expected");
_2:
    if(isAddOperator()) {
        goto _1;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstTerm = ConstFactor {MulOperator ConstFactor}.
bool ModuleCompiler::isConstTerm() {
//_0:
    if(isConstFactor()) {
        goto _1;
    }
    return false;
_1:
    if(isMulOperator()) {
        goto _2;
    }
    goto _end;
_2:
    if(isConstFactor()) {
        goto _1;
    }
    return erMessage("Constant Factor expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstFactor = number | string | NIL | TRUE | FALSE |
//             ConstSet | designator [ActualParameters] | "(" ConstExpression ")" | "~" ConstFactor.
bool ModuleCompiler::isConstFactor() {
//_0:
    if(isKeyWord("NIL")) {
        goto _end;
    }
    if(isKeyWord("TRUE")) {
        goto _end;
    }
    if(isKeyWord("FALSE")) {
        goto _end;
    }
    if(isString()) {
        goto _end;
    }
    if(isNumber()) {
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
        goto _4;
    }
    return false;
_1:
    if(isConstExpression()) {
        goto _2;
    }
    return erMessage("Constant Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return erMessage("Right bracket expected");
_3:
    if(isConstFactor()) {
        goto _end;
    }
    return erMessage("Constant Factor expected");
_4:
    if(isActualParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return true;
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
    if(isConstExpression()) {
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
    if(isConstExpression()) {
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
    if(isIdentdef()) {
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
    if(isType()) {
        goto _end;
    }
    return erMessage("Type value expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// type = qualident | ArrayType | RecordType | PointerType | ProcedureType.
bool ModuleCompiler::isType() {
//_0:
    if(isArrayType()) {
        goto _end;
    }
    if(isRecordType()) {
        goto _end;
    }
    if(isPointerType()) {
        goto _end;
    }
    if(isProcedureType()) {
        goto _end;
    }
    if(isQualident()) {
        goto _end;
    }
    return false;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ArrayType = ARRAY length {"," length} OF type.
// length = ConstExpression.
bool ModuleCompiler::isArrayType() {
//_0:
    if(isKeyWord("ARRAY")) {
        goto _1;
    }
    return false;
_1:
    if(isConstExpression()) {
        goto _2;
    }
    return erMessage("Constant Expression expected");
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
    return erMessage("Symbol ',' or Key word OF expected");
_3:
    if(isType()) {
        goto _end;
    }
    return erMessage("Type value expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// RecordType = RECORD ["(" BaseType ")"] [FieldListSequence] END.
// BaseType = qualident.
// FieldListSequence = FieldList {";" FieldList}.
bool ModuleCompiler::isRecordType() {
//_0:
    if(isKeyWord("RECORD")) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isFieldList()) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Symbol '(' or Key word END or Field list expected");
_2:
    if(isQualident()) {
        goto _3;
    }
    return erMessage("Qualident expected");
_3:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _4;
    }
    return erMessage("Symbol ')' expected");
_4:
    if(isFieldList()) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Key word END or Field list expected");
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
    return erMessage("Key word END or Symbol ';' expected");
_6:
    if(isFieldList()) {
        goto _5;
    }
    return erMessage("Field list expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// FieldList = IdentList ":" type.
// IdentList = identdef {"," identdef}.
bool ModuleCompiler::isFieldList() {
//_0:
    if(isIdentdef()) {
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
        goto _1;
    }
    return erMessage("Idendef expected");
_3:
    if(isType()) {
        goto _end;
    }
    return erMessage("Type expected");
_end:
    return true;
}


//-----------------------------------------------------------------------------
// PointerType = POINTER TO type.
bool ModuleCompiler::isPointerType() {
//_0:
    if(isKeyWord("POINTER")) {
        goto _1;
    }
    return false;
_1:
    if(isKeyWord("TO")) {
        goto _2;
    }
    return erMessage("POINTER TO expected");
_2:
    if(isType()) {
        goto _end;
    }
    return erMessage("Type expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureType = PROCEDURE [FormalParameters].
bool ModuleCompiler::isProcedureType() {
//_0:
    if(isKeyWord("PROCEDURE")) {
        goto _1;
    }
    return false;
_1:
    if(isFormalParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// FormalParameters = "(" [FPSection {";" FPSection}] ")" [":" qualident].
bool ModuleCompiler::isFormalParameters() {
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
    if(isFPSection()) {
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
        goto _end;
    }
    return erMessage("Qualident expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// FPSection = [VAR] ident {"," ident} ":" FormalType.
// FormalType = {ARRAY OF} qualident.
bool ModuleCompiler::isFPSection() {
//_0:
    if(isKeyWord("VAR")) {
        goto _1;
    }
    if(isIdent()) {
        goto _2;
    }
    return false;
_1:
    if(isIdent()) {
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
        goto _end;
    }
    return erMessage("Key word ARRAY or Qualident expected");
_4:
    if(isKeyWord("OF")) {
        goto _5;
    }
    return erMessage("Key word OF expected");
_5:
    if(isQualident()) {
        goto _end;
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
    if(isIdentdef()) {
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
        goto _1;
    }
    return erMessage("Idendef expected");
_3:
    if(isType()) {
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
    if(isProcedureHeading()) {
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
bool ModuleCompiler::isProcedureHeading() {
//_0:
    if(isKeyWord("PROCEDURE")) {
        goto _1;
    }
    return false;
_1:
    if(isIdentdef()) {
        goto _2;
    }
    return erMessage("Identdef expected");
_2:
    if(isFormalParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureBody = DeclarationSequence [BEGIN StatementSequence]
//                 [RETURN expression] END.
bool ModuleCompiler::isProcedureBody() {
//_0:
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
    if(isStatementSequence()) {
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
    if(isExpression()) {
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
bool ModuleCompiler::isStatementSequence() {
//_0:
    if(isStatement()) {
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
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
    goto _end;
_2:
    if(isStatement()) {
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
    return true;
}

//-----------------------------------------------------------------------------
// statement = [assignment | ProcedureCall | IfStatement | CaseStatement |
//             WhileStatement | RepeatStatement | ForStatement].
bool ModuleCompiler::isStatement() {
    if(isIfStatement()) {
        return true;
    }
    if(isCaseStatement()) {
        return true;
    }
    if(isWhileStatement()) {
        return true;
    }
    if(isRepeatStatement()) {
        return true;
    }
    if(isForStatement()) {
        return true;
    }
    ///if(isAssignmentOrProcedureCall()) {
    ///    return true;
    ///}
    if(isAssignment()) {
        return true;
    }
    if(isProcedureCall()) {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// assignment = designator ":=" expression.
bool ModuleCompiler::isAssignment() {
    Location l;
    storeLocation(l);
    /// Тестовая точка входа в правило
    ///std::cout << "----> isAssignment" << std::endl;
//_0:
    if(isDesignator()) {
        goto _1;
    }
    restoreLocation(l);
    return false;
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
    return false;
    // return erMessage("':=' expected");
_2:
    if(isExpression()) {
        goto _end;
    }
    return erMessage("Expression expected");
_end:
    /// Тестовая точка выхода из правила
    ///std::cout << "isAssignmentOrProcedure ---->" << std::endl;
    return true;
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
bool ModuleCompiler::isIfStatement() {
//_0:isAssignmentOrProcedure
    if(isKeyWord("IF")) {
        goto _1;
    }
    return false;
_1:
    if(isExpression()) {
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isKeyWord("THEN")) {
        goto _3;
    }
    return erMessage("Key word THEN expected");
_3:
    if(isStatementSequence()) {
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
    return erMessage("Statement Sequence or ELSIF or ELSE or END expected");
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
    return erMessage("ELSIF or ELSE or END expected");
_5:
    if(isStatementSequence()) {
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or END expected");
_6:
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("END expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// CaseStatement = CASE expression OF case {"|" case} END.
// case = [CaseLabelList ":" StatementSequence].
// CaseLabelList = LabelRange {"," LabelRange}.
// LabelRange = label [".." label].
// label = integer | string | qualident.
bool ModuleCompiler::isCaseStatement() {
//_0:
    if(isKeyWord("CASE")) {
        goto _1;
    }
    return false;
_1:
    if(isExpression()) {
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isKeyWord("OF")) {
        goto _3;
    }
    return erMessage("Key word OF expected");
_3:
    if(isInteger()) {
        goto _4;
    }
    if(isString()) {
        goto _4;
    }
    if(isQualident()) {
        goto _4;
    }
    return erMessage("Integer or String or Qualified Ident expected");
_4:
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
    return erMessage("'..' or ',' or ':' expected");
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
    return erMessage("Integer or String or Qualified Ident expected");
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
    return erMessage("',' or ':' expected");
_7:
    if(isStatementSequence()) {
        goto _8;
    }
    if(isSymbol(moduleStr[pos], '|')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or '|' or END expected");
_8:
    if(isSymbol(moduleStr[pos], '|')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("'|' or END expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// WhileStatement = WHILE expression DO StatementSequence
//                 {ELSIF expression DO StatementSequence} END.
bool ModuleCompiler::isWhileStatement() {
    //_0:
        if(isKeyWord("WHILE")) {
            goto _1;
        }
        return false;
    _1:
        if(isExpression()) {
            goto _2;
        }
        return erMessage("Expression expected");
    _2:
        if(isKeyWord("DO")) {
            goto _3;
        }
        return erMessage("Key word DO expected");
    _3:
        if(isStatementSequence()) {
            goto _4;
        }
        if(isKeyWord("ELSIF")) {
            goto _1;
        }
        if(isKeyWord("END")) {
            goto _end;
        }
        return erMessage("Statement Sequence or ELSIF or END expected");
    _4:
        if(isKeyWord("ELSIF")) {
            goto _1;
        }
        if(isKeyWord("END")) {
            goto _end;
        }
        return erMessage("ELSIF or END expected");
    _end:
        return true;
}

//-----------------------------------------------------------------------------
// RepeatStatement = REPEAT StatementSequence UNTIL expression.
bool ModuleCompiler::isRepeatStatement() {
//_0:
    if(isKeyWord("REPEAT")) {
        goto _1;
    }
    return false;
_1:
    if(isStatementSequence()) {
        goto _2;
    }
    if(isKeyWord("UNTIL")) {
        goto _3;
    }
    return erMessage("Statement Sequence or UNTIL expected");
_2:
    if(isKeyWord("UNTIL")) {
        goto _3;
    }
    return erMessage("Key word UNTIL expected");
_3:
    if((isExpression())) {
        goto _end;
    }
    return erMessage("Expression expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ForStatement = FOR ident ":=" expression TO expression [BY ConstExpression]
//                DO StatementSequence END.
bool ModuleCompiler::isForStatement() {
//_0:
    if(isKeyWord("FOR")) {
        goto _1;
    }
    return false;
_1:
    if((isIdent())) {
        goto _2;
    }
    return erMessage("Identifier expected");
_2:
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        goto _3;
    }
    return erMessage("':=' expected");
_3:
    if((isExpression())) {
        goto _4;
    }
    return erMessage("Expression expected");
_4:
    if(isKeyWord("TO")) {
        goto _5;
    }
    return erMessage("Key word TO expected");
_5:
    if((isExpression())) {
        goto _6;
    }
    return erMessage("Expression expected");
_6:
    if(isKeyWord("BY")) {
        goto _7;
    }
    if(isKeyWord("DO")) {
        goto _9;
    }
    return erMessage("Key word BY or DO expected");
_7:
    if((isConstExpression())) {
        goto _8;
    }
    return erMessage("Constant Expression expected");
_8:
    if(isKeyWord("DO")) {
        goto _9;
    }
    return erMessage("Key word DO expected");
_9:
    if(isStatementSequence()) {
        goto _10;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or END expected");
_10:
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("END expected");
_end:
    return true;
}


//-----------------------------------------------------------------------------
// expression = SimpleExpression [relation SimpleExpression].
bool ModuleCompiler::isExpression() {
//_0:
    std::string tmpValue = "";
    if(isSimpleExpression()) {
        goto _1;
    }
    return false;
_1:
    if(isRelation()) {
        goto _2;
    }
    goto _end;
_2:
    if(isSimpleExpression()) {
        goto _end;
    }
    return erMessage("Simple Expression expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// SimpleExpression = ["+" | "-"] term {AddOperator term}.
bool ModuleCompiler::isSimpleExpression() {
//_0:
    std::string tmpValue = "";
    if(isSymbol(moduleStr[pos], '+')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '-')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isTerm()) {
        goto _2;
    }
    return false;
_1:
    if(isTerm()) {
        goto _2;
    }
    return erMessage("Term expected");
_2:
    if(isAddOperator()) {
        goto _1;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// term = factor {MulOperator factor}.
bool ModuleCompiler::isTerm() {
//_0:
    if(isFactor()) {
        goto _1;
    }
    return false;
_1:
    if(isMulOperator()) {
        goto _2;
    }
    goto _end;
_2:
    if(isFactor()) {
        goto _1;
    }
    return erMessage("Factor expected");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// factor = number | string | NIL | TRUE | FALSE |
//       set | designator [ActualParameters] | "(" expression ")" | "~" factor.
bool ModuleCompiler::isFactor() {
//_0:
    if(isKeyWord("NIL")) {
        goto _end;
    }
    if(isKeyWord("TRUE")) {
        goto _end;
    }
    if(isKeyWord("FALSE")) {
        goto _end;
    }
    if(isString()) {
        goto _end;
    }
    if(isNumber()) {
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
        goto _4;
    }
    return false;
_1:
    if(isExpression()) {
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return erMessage("Right bracket expected");
_3:
    if(isFactor()) {
        goto _end;
    }
    return erMessage("Factor expected");
_4:
    if(isActualParameters()) {
        goto _end;
    }
    goto _end;
_end:
    return true;
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
    if(isExpression()) {
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
    if(isExpression()) {
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
    if(isExpression()) {
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
    if(isExpression()) {
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
