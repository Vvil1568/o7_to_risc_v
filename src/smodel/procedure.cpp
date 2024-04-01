#include "procedure.h"

#include "variable.h"

// ����� ����� ���������� ���������� � ����������
void ProcContext::debugOut() {
    std::cout << "PROCEDURE ";
    // ���������� ���������� � ���� ����������
    std::cout << "RESULT ";
    resultType->debugOut();
}
// ���������� ������������ ���������
void ProcContext::AddNamedArtefact(std::string name, Context* context) {
    NamedArtefact* tmpNamed = new NamedArtefact{ name, context, false };
    namedArtefacts.push_back(tmpNamed);
    // �������� �������� �������� ������ �� ����������� ��������
    context->setNamedArtefact(tmpNamed);
}

// ���������� ����� �����
void ProcContext::ComputeStackFrame() {
    stackFrameSize = 0;
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            // ���� ��� �� �������� - ������ ���������� ���������, � ����� �������� �� �����
            if (dynamic_cast<ArgVarContext*>(artefact->getContext()) == nullptr) {
                var->setOnStack();
            }
            Register* reg = var->getAssignedReg();
            if (var->isOnStack()) {
                stackFrameSize += var->getType()->getTypeSize();
                var->setStackOffset(-stackFrameSize);
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
    label = CodeGenContext::pushContext(name);
    CodeGenContext::popContext();
}

// ��������� ������������� ����
void ProcContext::GenerateAsmCode() {
    std::string procLabelName = CodeGenContext::pushContext(name);
    CodeGenContext::addCodeLine("#��������� \"" + this->name + "\"");
    for (NamedArtefact* artefact : namedArtefacts) {
        if (VarContext* var = dynamic_cast<VarContext*>(artefact->getContext())) {
            Register* reg = var->getAssignedReg();
            if (var->isOnStack()) {
                CodeGenContext::addCodeLine("#���������� \"" + var->getName() + "\" ������������� �� ����� �� ������ sp" + std::to_string(var->getStackOffset()));
            }
            else {
                CodeGenContext::addCodeLine("#���������� \"" + var->getName() + "\" ������������� � �������� " + reg->getName());
            }
        }
    }
    if (resultType != nullptr) {
        if (resultType->getTypeName() == "REAL") {
            CodeGenContext::addCodeLine("#������������ �������� ����� ������������� � �������� fa0");
        }
        else {
            CodeGenContext::addCodeLine("#������������ �������� ����� ������������� � �������� a0");
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
    CodeGenContext::addCodeLine(procLabelName + ":");
    CodeGenContext::getInstance().codeIndent += "    ";
    CodeGenContext::addCodeLine("#�������� �� ����� ������ ��� ������ �������");
    CodeGenContext::addCodeLine("addi sp sp "+ std::to_string(stackFrameSize));
    for (StatementContext* statement : GetStatementSequence()) {
        statement->generateAsmCode();
    }

    Register* resReg;
    if (getResultType()->getTypeName() == "REAL") {
        resReg = RegisterPool::fa0;
    }
    else {
        resReg = RegisterPool::a0;
    }
    returnExpr->AssignReg(resReg);
    returnExpr->generateAsmCode();
    if (stackFrameSize != 0) {
        CodeGenContext::addCodeLine("#������������ �� ����� ������, ���������� ��� ������ �������");
        CodeGenContext::addCodeLine("addi sp sp " + std::to_string(-stackFrameSize));
    }
    CodeGenContext::addCodeLine("#����� �� ������������");
    CodeGenContext::addCodeLine("ret");

    for (ProcContext* procedure : procedures) {
        procedure->GenerateAsmCode();
    }
    CodeGenContext::popContext();
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