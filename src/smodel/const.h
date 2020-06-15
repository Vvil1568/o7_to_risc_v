#ifndef CONST_H
#define CONST_H

#include "context.h"

// Класс, определяющий контекст константы.
// Предполагается, что в языке существуют константы основных типов
class ConstContext: public Context {
public:
    // Вывод отладочной информации о контексте импорта
    virtual void debugOut();
};

// Класс, определяющий контекст целочисленной константы.
class ConstIntContext: public ConstContext {
public:
    // Создание целочисленной константы
    ConstIntContext(int v): value(v) {}

    // Вывод отладочной информации о целочисленной константе
    virtual void debugOut();
private:
    // Значение константы
    int value;
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
private:
    // Значение константы
    T value;
};

#endif // CONST_H
