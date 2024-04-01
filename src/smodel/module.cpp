#include "module.h"

Module::Module()
{

}


void Module::CompileModule() {
    CodeGenContext::pushContext(name);
    CodeGenContext::addCodeLine("#������ \"" + this->name + "\"");
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            Register* reg = var->getAssignedReg();
            /*
            if (var->isOnStack()) {
                int addr = StackController::getInstance().addModuleVariable(var->getName(), var->getType()->getTypeSize());
                CodeGenContext::addCodeLine("#���������� \"" + var->getName() + "\" ������������� �� ����� �� ������ " +std::to_string(addr) + " ������������ ����� �����");
            }
            else {
                CodeGenContext::addCodeLine("#���������� \"" + var->getName() + "\" ������������� � �������� "+reg->getName());
            }
            */
            CodeGenContext::addCodeLine("#���������� \"" + var->getName() + "\" ������������� � �������� " + reg->getName());
        }
    }
    std::vector<ProcContext*> procedures;
    for (NamedArtefact* artefact : namedArtefacts) {
        if (ProcContext* procedure = dynamic_cast<ProcContext*>(artefact->getContext())) {
            procedures.push_back(procedure);
        }
    }
    for (ProcContext* procedure : procedures) {
        procedure->ComputeStackFrame();
    }
    CodeGenContext::addCodeLine("module_" + name + ":");
    CodeGenContext::getInstance().codeIndent += "    ";
    for (StatementContext* statement : GetStatementSequence()) {
        statement->generateAsmCode();
    }
    CodeGenContext::addCodeLine("#���������� ��������� � ����� 0 (��������� ����� 10)");
    CodeGenContext::addCodeLine("li a7 10");
    CodeGenContext::addCodeLine("ecall");

    for (ProcContext* procedure : procedures) {
        procedure->GenerateAsmCode();
    }
}

// ����� ���������� ���������� � ������
void Module::debugOut() {
    std::cout << "Module " << name << std::endl;
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
