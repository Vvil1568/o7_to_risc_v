#include "commondata.h"

// Добавление именованного артефакта
void CommonData::AddNamedArtefact(std::string name, Context* context, bool access) {
    NamedArtefact* tmpNamed = new NamedArtefact{ name, context, access };
    namedArtefacts.push_back(tmpNamed);
    // Контекст получает обратную ссылку на именованный артефакт
    context->setNamedArtefact(tmpNamed);
}

TypeContext* CommonData::GetTypeFromName(std::string name)
{
    for (NamedArtefact* artefact : namedArtefacts) {
        if (artefact->getName() != name) continue;
        if (TypeContext* type = dynamic_cast<TypeContext*>(artefact->getContext())) {
            return type;
        }
    }
    return nullptr;
}

ConstContext* CommonData::GetConstFromName(std::string name)
{
    for (NamedArtefact* artefact : namedArtefacts) {
        if (artefact->getName() != name) continue;
        if (ConstContext* type = dynamic_cast<ConstContext*>(artefact->getContext())) {
            return type;
        }
    }
    return nullptr;
}

VarContext* CommonData::GetVarFromName(std::string name)
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

std::vector<StatementContext*> CommonData::GetStatementSequence() {
    return this->statements;
}