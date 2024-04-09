#include "procedure.h"

#include "variable.h"

// Вывод общей отладочной информации о переменной
void ProcContext::debugOut() {
    std::cout << "PROCEDURE ";
    // Добавление информации о типе переменной
    std::cout << "RESULT ";
    resultType->debugOut();
}
// Добавление именованного артефакта
void ProcContext::AddNamedArtefact(std::string name, Context* context) {
    NamedArtefact* tmpNamed = new NamedArtefact{ name, context, false };
    namedArtefacts.push_back(tmpNamed);
    // Контекст получает обратную ссылку на именованный артефакт
    context->setNamedArtefact(tmpNamed);
}

// Вычисление кадра стека
void ProcContext::ComputeStackFrame() {
    if (isCompute) return;
    isCompute = true;
    stackFrameSize = 0;
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            // Если это не аргумент - значит переменная процедуры, а такие хранятся на стеке
            if (dynamic_cast<ArgVarContext*>(artefact->getContext()) == nullptr) {
                var->setOnStack();
            }
            else {
                var->getAssignedReg();
            }
            if (var->isInMemory()) {
                var->setMemoryOffset(stackFrameSize);
                stackFrameSize += var->getType()->getTypeSize();
            }
        }
    }
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            if (!var->isInMemory()) {
                RegisterPool::getInstance().freeRegister(var->getAssignedReg());
            }
        }
    }
    if (label == "") {
        label = CodeGenContext::getLabelName(name);
    }
}

// Генерация ассемблерного кода
void ProcContext::GenerateAsmCode() {
    if (isASMed) return;
    CodeGenContext::getInstance().isInProc = true;
    isASMed = true;
    bool labelGenerated = label != "";
    if (!labelGenerated) {
        label = CodeGenContext::pushContext(name);
    }
    CodeGenContext::addCodeComment("Procedure \"" + this->name + "\"");
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            if (var->isInMemory()) {
                CodeGenContext::addCodeComment("Variable \"" + var->getName() + "\" is placed on the stack with the address sp+" + std::to_string(var->getMemoryOffset()));
            }
            else {
                Register* reg = var->getAssignedReg();
                CodeGenContext::addCodeComment("Variable \"" + var->getName() + "\" is placed in " + reg->getName() +" register");
            }
        }
    }
    if (resultType != nullptr) {
        if (getResultType()->getTypeName() == "REAL") {
            resReg = RegisterPool::fa1;
        }
        else {
            resReg = RegisterPool::a1;
        }
        CodeGenContext::addCodeComment("Return value will be placed in " + resReg->getName() +" register");
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
    CodeGenContext::addCodeLine(label + ":");
    CodeGenContext::getInstance().codeIndent += "    ";
    for (StatementContext* statement : GetStatementSequence()) {
        statement->generateAsmCode();
    }

    if (getResultType() != nullptr) {
        returnExpr->AssignReg(resReg);
        returnExpr->generateAsmCode();
    }
    CodeGenContext::addCodeComment("Return from the subroutine");
    CodeGenContext::addCodeLine("ret");

    for (ProcContext* procedure : procedures) {
        procedure->GenerateAsmCode();
    }
    CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
    if (!labelGenerated) {
        CodeGenContext::popContext();
    }
    CodeGenContext::getInstance().isInProc = false;
}

std::vector<ArgVarContext*> ProcContext::GetFormalParams() {
    std::vector<ArgVarContext*> args;
    for (NamedArtefact* artefact : namedArtefacts) {
        if (ArgVarContext* type = dynamic_cast<ArgVarContext*>(artefact->getContext())) {
            args.push_back(type);
        }
    }
    return args;
}