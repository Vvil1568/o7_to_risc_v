#include "operand.h"

Operand::Operand()
{

}

// Вывод отладочной информации
void Operand::debugOut() {
    std::cout << "OPERAND ";
    context->debugOut();
}
