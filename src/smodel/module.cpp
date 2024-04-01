#include "module.h"

Module::Module()
{

}


void Module::CompileModule() {
    CodeGenContext::pushContext(name);
    CodeGenContext::addCodeLine("#Модуль \"" + this->name + "\"");
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            Register* reg = var->getAssignedReg();
            /*
            if (var->isOnStack()) {
                int addr = StackController::getInstance().addModuleVariable(var->getName(), var->getType()->getTypeSize());
                CodeGenContext::addCodeLine("#Переменная \"" + var->getName() + "\" располагается на стеке по адресу " +std::to_string(addr) + " относительно конца стека");
            }
            else {
                CodeGenContext::addCodeLine("#Переменная \"" + var->getName() + "\" располагается в регистре "+reg->getName());
            }
            */
            CodeGenContext::addCodeLine("#Переменная \"" + var->getName() + "\" располагается в регистре " + reg->getName());
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
    CodeGenContext::addCodeLine("#Завершение программы с кодом 0 (системный вызов 10)");
    CodeGenContext::addCodeLine("li a7 10");
    CodeGenContext::addCodeLine("ecall");

    for (ProcContext* procedure : procedures) {
        procedure->GenerateAsmCode();
    }
}

// Вывод отладочной информации о модуле
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
