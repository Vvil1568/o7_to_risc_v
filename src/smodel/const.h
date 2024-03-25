#ifndef CONST_H
#define CONST_H

#include "context.h"

// Класс, определяющий контекст константы.
// Предполагается, что в языке существуют константы основных типов
class ConstContext: public Context {
public:
    virtual ConstContext* getValue(){
        return this;
    }
    // Вывод отладочной информации о контексте импорта
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "UNDEFINED";
    }
};

// Класс, определяющий контекст целочисленной константы.
class ConstIntContext: public ConstContext {
public:
    // Создание целочисленной константы
    ConstIntContext(int v) : value(v) { }

    // Вывод отладочной информации о целочисленной константе
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "INT";
    }
    int getIntValue() {
        return value;
    }
private:
    // Значение константы
    int value;
};

// Класс, определяющий контекст вещественной константы.
class ConstRealContext : public ConstContext {
public:
    // Создание вещественной константы
    ConstRealContext(double v) : value(v) { }

    // Вывод отладочной информации о вещественной константе
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "REAL";
    }
    double getRealValue() {
        return value;
    }
private:
    // Значение константы
    double value;
};

// Класс, определяющий контекст булевой константы.
class ConstBooleanContext : public ConstContext {
public:
    // Создание булевой константы
    ConstBooleanContext(bool v) : value(v) { }

    // Вывод отладочной информации о булевой константе
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "BOOL";
    }
    bool getBoolValue() {
        return value;
    }
private:
    // Значение константы
    bool value;
};

// Класс, определяющий контекст строковой константы.
class ConstStringContext : public ConstContext {
public:
    // Создание строковой константы
    ConstStringContext(std::string v) : value(v) { }

    // Вывод отладочной информации о строковой константе
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "STRING";
    }
    std::string getStrValue() {
        return value;
    }
private:
    // Значение константы
    std::string value;
};

// Класс, определяющий контекст константы NIL.
class ConstNilContext : public ConstContext {
public:
    // Создание константы NIL
    ConstNilContext() { }

    // Вывод отладочной информации о константе NIL
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "NIL";
    }
};

// Класс, определяющий контекст ошибки.
class ConstErrContext : public ConstContext {
public:
    // Создание ошибки
    ConstErrContext(std::string errMsg) : errMsg(errMsg) { }

    // Вывод отладочной информации о константе ошибки
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "ERR";
    }
    std::string getErrMsg() {
        return errMsg;
    }
private:
    // Сообщение об ошибке
    std::string errMsg;
};

// Класс, определяющий контекст константного выражения.
class ConstExprContext : public ConstContext {
public:
    // Создание константного выражения
    ConstExprContext(ConstContext* leftOp, ConstContext* rightOp, std::string operName) {
        left = leftOp;
        right = rightOp;
        this->operName = operName;
    }
    virtual ConstContext* getValue();
    // Вывод отладочной информации о константном выражении
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "EXPR";
    }
private:
    // Левый операнд
    ConstContext* left;
    // Правый операнд
    ConstContext* right;
    // Название оператора
    std::string operName;
};

// Класс, определяющий контекст унарного константного выражения.
class ConstUnarExprContext : public ConstContext {
public:
    // Создание константного выражения
    ConstUnarExprContext(ConstContext* operand, std::string operName) {
        this->operand = operand;
        this->operName = operName;
    }
    virtual ConstContext* getValue();
    // Вывод отладочной информации о константном выражении
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "EXPR";
    }
private:
    // Операнд
    ConstContext* operand;
    // Название оператора
    std::string operName;
};

// Класс, определяющий контекст константы.
template<typename T>
class ConstValueContext: public ConstContext {
public:
    ///ConstValueContext();
    // Создание целочисленной константы
    ConstValueContext(T v): value(v) {}

    // Вывод отладочной информации о целочисленной константе
    virtual void debugOut();
    // Информации о типе контекста
    virtual std::string getType() {
        return "VALUE";
    }
private:
    // Значение константы
    T value;
};

#endif // CONST_H
