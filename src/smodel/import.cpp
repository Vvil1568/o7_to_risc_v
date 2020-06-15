#include "import.h"

// Создание контекста импорта с установкой имен
ImportContext::ImportContext(std::string importedName, std::string alias):
    importedName(importedName),
    alias(alias)
{

}

// Вывод отладочной информации о контексте импорта
void ImportContext::debugOut() {
    std::cout << "IMPORT " << alias << " := " << importedName; // << std::endl;
}
