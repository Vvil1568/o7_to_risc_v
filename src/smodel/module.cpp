#include "module.h"

std::string CommonData::moduleName{""};  // имя модуля
// Список артефактов с зарезервированными именами
std::vector<NamedArtefact*> CommonData::reservedNamedArtefacts;
// Список именованных артефактов, порождаемых в модуле
std::vector<NamedArtefact*> CommonData::namedArtefacts;

Module::Module()
{

}

// Добавление именованного артефакта
void Module::AddNamedArtefact(std::string name, Context* context, bool access) {
    NamedArtefact* tmpNamed = new NamedArtefact{name, context, access};
    namedArtefacts.push_back(tmpNamed);
    // Контекст получает обратную ссылку на именованный артефакт
    context->setNamedArtefact(tmpNamed);
}

TypeContext* Module::GetTypeFromName(std::string name)
{
    for (NamedArtefact* artefact : namedArtefacts) {
        if (artefact->getName() != name) continue;
        if (TypeContext* type = dynamic_cast<TypeContext*>(artefact->getContext())) {
            return type;
        }
    }
    return nullptr;
}

ConstContext* Module::GetConstFromName(std::string name)
{
    for (NamedArtefact* artefact : namedArtefacts) {
        if (artefact->getName() != name) continue;
        if (ConstContext* type = dynamic_cast<ConstContext*>(artefact->getContext())) {
            return type;
        }
    }
    return nullptr;
}

VarContext* Module::GetVarFromName(std::string name)
{
    for (NamedArtefact* artefact : namedArtefacts) {
        if (artefact->getName() != name) continue;
        if (VarContext* type = dynamic_cast<VarContext*>(artefact->getContext())) {
            return type;
        }
    }
    return nullptr;
}

void CommonData::SetStatementSequence(std::vector<StatementContext*> statements) {
    this->statements = statements;
}

// Вывод отладочной информации о модуле
void Module::debugOut() {
    std::cout << "Module " << CommonData::moduleName << std::endl;
    std::cout << "Reserved artefats: " << std::endl;
    std::cout << "------------------ " << std::endl;
    std::cout << "Artefats: " << std::endl;
    std::cout << "--------- " << std::endl;
    for(NamedArtefact* artefact: namedArtefacts) {
        std::cout << artefact->getName() << (artefact->getAccess()? "*: ":": ");
        artefact->getContext()->debugOut();
        std::cout << std::endl;
    }
}
