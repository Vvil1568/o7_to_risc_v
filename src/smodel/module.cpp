#include "module.h"

bool Module::putModuleVarsIntoData = false;

Module::Module()
{

}

void Module::CompileModule() {
    CodeGenContext::pushContext(name);
    CodeGenContext::addCodeComment("Module \"" + this->name + "\"");
    CodeGenContext::addDataComment("\"" + this->name + "\"" + " module data");
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            if (Module::putModuleVarsIntoData || RegisterPool::getInstance().getRegCount(RegType::S) == 0 || var->getType()->getTypeName() == "RECORD" || (var->getType()->getTypeName().size() >= 5 && var->getType()->getTypeName().substr(0, 5) == "ARRAY")) {
                std::string label = CodeGenContext::getLabelName(var->getName());
                var->setOnData(label);
                CodeGenContext::addDataComment("Space for the "+ var->getName() + " variable:");
                CodeGenContext::addDataLine(label+": .space "+std::to_string(var->getType()->getTypeSize()));
                CodeGenContext::addCodeComment("Variable \"" + var->getName() + "\" is placed in the data segment and is marked with the " + label +" label");
            }
            else {
                Register* reg = var->getAssignedReg();
                CodeGenContext::addCodeComment("Variable \"" + var->getName() + "\" is placed in " + reg->getName()+" register");
            }
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
    CodeGenContext::addCodeComment("Finishing the program with 0 return code (system call 10)");
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
