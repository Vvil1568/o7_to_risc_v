#include "module.h"

std::string CommonData::moduleName{""};  // ��� ������
// ������ ���������� � ������������������ �������
std::vector<NamedArtefact*> CommonData::reservedNamedArtefacts;
// ������ ����������� ����������, ����������� � ������
std::vector<NamedArtefact*> CommonData::namedArtefacts;

Module::Module()
{

}

// ���������� ������������ ���������
void Module::AddNamedArtefact(std::string name, Context* context, bool access) {
    NamedArtefact* tmpNamed = new NamedArtefact{name, context, access};
    namedArtefacts.push_back(tmpNamed);
    // �������� �������� �������� ������ �� ����������� ��������
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

void Module::CompileModule() {
    CodeGenContext::addCodeLine("#������ \"" + this->moduleName + "\"");
    CodeGenContext::addCodeLine("module_" + moduleName + ":");
    CodeGenContext::getInstance().codeIndent += "    ";
    for (StatementContext* statement : GetStatementSequence()) {
        statement->generateAsmCode();
    }
}

void CommonData::SetStatementSequence(std::vector<StatementContext*> statements) {
    this->statements = statements;
}

std::vector<StatementContext*> CommonData::GetStatementSequence() {
    return this->statements;
}

// ����� ���������� ���������� � ������
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
