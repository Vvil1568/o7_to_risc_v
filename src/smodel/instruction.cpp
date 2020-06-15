#include "instruction.h"

// Конструктор для незаполненной команды
Instruction::Instruction(): opc(Nop),
    argument1(nullptr), argument2(nullptr),
    result(nullptr),
    next(nullptr)
{}

// Конструктор списка команд
InstructionList::InstructionList(): opd(nullptr)
{}
