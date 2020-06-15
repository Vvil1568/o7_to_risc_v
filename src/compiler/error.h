#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
// Класс, определяющий обработчик ошибок и дополнительные утилиты,
// контролирующие корректное перемещение по обрабатываемой строке
class Error {
public:
    // Конструктор обработчика ошибок

    // Выдача сообщения об ошибке с установкой
    bool erMessage(std::string&& str);
private:
    // Фиксаторы позиции, строки и столбца
    int oldPos;
    int oldLine;
    int oldColumn;
};

#endif // ERROR_H
