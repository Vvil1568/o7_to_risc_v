#include "error.h"

//-----------------------------------------------------------------------------
// Выдача сообщения об ошибке
bool Error::erMessage(std::string&& str) {
    // Вывод сообщения об ошибке
    std::cout << "ERROR (" << pos << "): ";
    std::cout << "It's not the name of Module" << std::endl;
    return false;
}
