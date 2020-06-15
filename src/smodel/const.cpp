#include <string>

#include "const.h"

// Вывод отладочной информации об общем контексте константы
void ConstContext::debugOut() {
    std::cout << "CONST ";
}

// Вывод отладочной информации о целочисленной константе
void ConstIntContext::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}

/*
// Вывод отладочной информации о константе
template<typename T>
void ConstValueContext<T>::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}
*/

// Вывод отладочной информации о булевской константе
template<>
void ConstValueContext<bool>::debugOut() {
    ConstContext::debugOut();
    std::cout << (value? "true":"false"); // << std::endl;
}

// Вывод отладочной информации о действительной константе
template<>
void ConstValueContext<double>::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}

// Вывод отладочной информации о символьной константе
template<>
void ConstValueContext<std::string>::debugOut() {
    ConstContext::debugOut();
    std::cout << "\"" << value << "\""; // << std::endl;
}
