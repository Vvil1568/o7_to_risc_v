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
            Register* reg = var->getAssignedReg();
            if (var->isOnStack()) {
                var->setStackOffset(-stackFrameSize);
                stackFrameSize += var->getType()->getTypeSize();
            }
        }
    }
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            Register* reg = var->getAssignedReg();
            if (!var->isOnStack()) {
                RegisterPool::getInstance().freeRegister(reg);
            }
        }
    }
    if (label == "") {
        label = CodeGenContext::pushContext(name);
        CodeGenContext::popContext();
    }
}

// Генерация ассемблерного кода
void ProcContext::GenerateAsmCode() {
    if (isASMed) return;
    isASMed = true;
    bool labelGenerated = label != "";
    if (!labelGenerated) {
        label = CodeGenContext::pushContext(name);
    }
    CodeGenContext::addCodeLine("#Процедура \"" + this->name + "\"");
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            Register* reg = var->getAssignedReg();
            if (var->isOnStack()) {
                CodeGenContext::addCodeLine("#Переменная \"" + var->getName() + "\" располагается на стеке по адресу sp" + std::to_string(var->getStackOffset()));
            }
            else {
                CodeGenContext::addCodeLine("#Переменная \"" + var->getName() + "\" располагается в регистре " + reg->getName());
            }
        }
    }
    if (resultType != nullptr) {
        if (resultType->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("#Возвращаемое значение будет располагаться в регистре fa0");
        }
        else {
            CodeGenContext::addCodeLine("#Возвращаемое значение будет располагаться в регистре a0");
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
    CodeGenContext::addCodeLine(label + ":");
    CodeGenContext::getInstance().codeIndent += "    ";
    for (StatementContext* statement : GetStatementSequence()) {
        statement->generateAsmCode();
    }

    if (getResultType() != nullptr) {
        Register* resReg;
        if (getResultType()->getTypeName() == "REAL") {
            resReg = RegisterPool::fa0;
        }
        else {
            resReg = RegisterPool::a0;
        }
        returnExpr->AssignReg(resReg);
        returnExpr->generateAsmCode();
    } //TODO тип возвращаемого значения, тип возвращаемого выражения должны совпадать и существовать
    CodeGenContext::addCodeLine("#Выход из подпрограммы");
    CodeGenContext::addCodeLine("ret");

    for (ProcContext* procedure : procedures) {
        procedure->GenerateAsmCode();
    }
    CodeGenContext::getInstance().codeIndent.resize(CodeGenContext::getInstance().codeIndent.size() - 4);
    if (!labelGenerated) {
        CodeGenContext::popContext();
    }
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