#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cctype>
#include "creator.h"

// Структура для временного хранения позиций во время разбора
struct Location {
    int pos;
    int line;
    int column;
};

// Класс, задающий компилятор модуля
class ModuleCompiler {
    // Данные для хранения текущего местоположения в тексте
    int pos;        // Позиция в тексте
    int line;       // Номер строки
    int column;     // Номер столбца

    const char* moduleStr = nullptr; // ссылка на строку с текстом обрабатываемого модуля
    ///int pos{0};    // позиция текущего символа в обрабатываемом модуле
    std::string lexValue;   // значение текущей подстроки, набранной в ходе парсинга

    // Координаты текущей позиции в тексте, номера текущей строки, номера текущего столбца
    // Location location{0,1,1}; -- в конструктор

    // Начальные установки параметров компилятора и его запуск
    //void Compile(const char* str); - внешняя функция...

    // Данные для хранения старого местоположения в тексте
    int oldPos;        // Позиция в тексте
    int oldLine;       // Номер строки
    int oldColumn;     // Номер столбца
    // Создатель элементов семантической модели
    Creator creator;
    Module curModule;
    //Get the qualident type
    TypeContext* getTypeFromQualident(std::string qualident);
public:
    // Конструктор, формирующий начальные установки параметров компилятора
    ModuleCompiler(const char* str);
    void InitParser(const char* str);

    // Module
    bool isModule();
    
    // ImportList
    bool isImportList();
    // DeclarationSequence
    bool isDeclarationSequence();
    // ConstDeclaration
    bool isConstDeclaration();
    // ConstExpression
    std::pair<bool, ConstContext*> isConstExpression();
    // SimpleConstExpression
    std::pair<bool, ConstContext*> isSimpleConstExpression();
    // ConstTerm
    std::pair<bool, ConstContext*> isConstTerm();
    // ConstFactor
    std::pair<bool, ConstContext*> isConstFactor();
    // ConstSet
    bool isConstSet();
    // ConstElement
    bool isConstElement();
    // TypeDeclaration
    bool isTypeDeclaration();
    // type
    std::pair<bool, TypeContext*> isType();
    // ArrayType
    std::pair<bool, TypeContext*> isArrayType();
    // RecordType
    std::pair<bool, TypeContext*> isRecordType();
    // FieldList
    bool isFieldList(TypeRecordContext* record);
    // PointerType
    std::pair<bool, TypeContext*> isPointerType();
    // ProcedureType
    std::pair<bool, ProcContext*> isProcedureType();
    // FormalParameters
    bool isFormalParameters(ProcContext*);
    // FPSection
    bool isFPSection(ProcContext* proc);
    // VariableDeclaration
    bool isVariableDeclaration();
    // ProcedureDeclaration
    bool isProcedureDeclaration();
    // ProcedureHeading
    std::pair<bool, ProcContext*> isProcedureHeading();
    // ProcedureBody
    bool isProcedureBody();
    // StatementSequence
    std::pair<bool, std::vector<StatementContext*>> isStatementSequence();
    // statement
    std::pair<bool, StatementContext*> isStatement();
    // assignment = designator ":=" expression.
    std::pair<bool, StatementContext*> isAssignment();
    // ProcedureCall = designator [ActualParameters].
    bool isProcedureCall();
    // IfStatement
    bool isIfStatement();
    // CaseStatement
    bool isCaseStatement();
    // WhileStatement
    bool isWhileStatement();
    // RepeatStatement
    bool isRepeatStatement();
    // ForStatement
    bool isForStatement();
    // expression
    std::pair<bool, ExprContext*> isExpression();
    // SimpleExpression
    std::pair<bool, ExprContext*> isSimpleExpression();
    // term
    std::pair<bool, ExprContext*> isTerm();
    // factor
    std::pair<bool, ExprContext*> isFactor();
    // designator
    bool isDesignator();
    // set
    bool isSet();
    // ActualParameters
    bool isActualParameters();

    //-----------------------------------------------------------------------------
    // Правила, определяющие лексический анализ
    //-----------------------------------------------------------------------------
    // Идентификатор без игнорируемых символов
    // Id = Letter {Letter | Digit}.
    bool isId();
    // Ident = Letter {Letter | Digit}.
    bool isIdent();
    // Identdef = ident ["*"].
    bool isIdentdef();
    // qualident = [ident "."] ident.
    bool isQualident();
    // KeyWord = Ident(str).
    bool isKeyWord(const std::string&& str);
    // AssignSymbol = ":="
    bool isAssignSymbol();
    // relation = "=" | "#" | "<" | "<=" | ">" | ">=" | IN | IS.
    bool isRelation();
    // AddOperator = "+" | "-" | OR.
    bool isAddOperator();
    // MulOperator = "*" | "/" | DIV | MOD | "&".
    bool isMulOperator();
    // number = integer | real.
    bool isNumber();
    // Real
    bool isReal();
    // digit {digit} | digit {hexDigit} "H".
    bool isInteger();
    // string = """ {character} """ | digit {hexDigit} "X".
    bool isString();
    // comment = "(*" {character} "*)".
    bool isComment();

    //-----------------------------------------------------------------------------
    // Правила, определяющие транслитерацию
    //-----------------------------------------------------------------------------
    bool isEndOfString(); // проверка на окончание разбираемой строки
    inline bool isLetter(char ch) {return bool(isalpha(int(ch)));}
    inline bool isDigit(char ch) {return bool(isdigit(int(ch)));}
    inline bool isHexDigit(char ch) {
        return (bool(isdigit(int(ch)))||(ch>='A' && ch <='H'));
    }
    inline bool isLetterOrDigit(char ch) {return bool(isalnum(int(ch)));}
    inline bool isSymbol(char ch, int const sym) {return bool(int(ch)==sym);}
    inline bool isOneSymbol(char ch) {return moduleStr[pos]==ch;}

    //-----------------------------------------------------------------------------
    // Вспомогательные утилиты
    //-----------------------------------------------------------------------------
    // Пропуск пробельных символов и комментариев
    // Ignore = {/ '\s' | '\n' | Comment }
    void ignore();
    // Проверка на принадлежность идентификатора множеству
    // ключевых или зарезервированных слов
    bool isKeyWordValue(std::string& ident);
    // Выдача сообщения об ошибке
    bool erMessage(std::string&& str);
    // Формирование типового тестового сообщения
    void testMessage(std::string str);
    // Временное сохранение текущей позиции
    inline void storeLocation(Location& l) {
        l.pos = pos;
        l.line = line;
        l.column = column;
    }
    // Восстановление текущей позиции
    inline void restoreLocation(Location& l) {
        pos = l.pos;
        line = l.line;
        column = l.column;
    }
    
};
#endif // PARSER_H
