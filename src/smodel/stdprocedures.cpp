#include "stdprocedures.h"
#include "creator.h"

STDProcedures::STDProcedures() {
	Creator creator;
	ProcContext* proc = new ProcContext();
	VarContext* argVar;
	RegisterPool::getInstance();
	proc->setProcedureName("WriteInt");
	argVar = creator.CreateArgVariable("n", creator.CreateTypeInt(), false);
	argVar->AssignReg(RegisterPool::a0);
	proc->AddNamedArtefact("n", argVar);
	std::vector<StatementContext*> statements;
	statements.push_back(creator.CreateSysCallStatement(1));
	proc->SetStatementSequence(statements);
	proc->setLabel("Out_WriteInt");
	procList["Out.WriteInt"] = proc;

	proc = new ProcContext();
	proc->setProcedureName("WriteStr");
	argVar = creator.CreateArgVariable("str", creator.CreateTypeArray(creator.CreateTypeChar(), 0), false);
	argVar->AssignReg(RegisterPool::a0);
	proc->AddNamedArtefact("str", argVar);
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(4));
	proc->SetStatementSequence(statements);
	proc->setLabel("Out_WriteStr");
	procList["Out.WriteStr"] = proc;

	proc = new ProcContext();
	proc->setProcedureName("ReadInt");
	argVar = creator.CreateArgVariable("n", creator.CreateTypeInt(), true);
	argVar->AssignReg(RegisterPool::a0);
	proc->AddNamedArtefact("n", argVar);
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(5));
	proc->SetStatementSequence(statements);
	proc->setLabel("In_ReadInt");
	procList["In.ReadInt"] = proc;

	proc = new ProcContext();
	proc->setProcedureName("ReadStr");
	argVar = creator.CreateArgVariable("str", creator.CreateTypeArray(creator.CreateTypeChar(), 0), true);
	argVar->AssignReg(RegisterPool::a0);
	proc->AddNamedArtefact("str", argVar);
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(8));
	proc->SetStatementSequence(statements);
	proc->setLabel("In_ReadStr");
	procList["In.ReadStr"] = proc;

	proc = new ProcContext();
	RegisterPool::getInstance();
	proc->setProcedureName("WriteReal");
	argVar = creator.CreateArgVariable("n", creator.CreateTypeReal(), false);
	argVar->AssignReg(RegisterPool::fa0);
	proc->AddNamedArtefact("n", argVar);
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(3));
	proc->SetStatementSequence(statements);
	proc->setLabel("Out_WriteReal");
	procList["Out.WriteReal"] = proc;

	proc = new ProcContext();
	RegisterPool::getInstance();
	proc->setProcedureName("ReadReal");
	argVar = creator.CreateArgVariable("n", creator.CreateTypeReal(), true);
	argVar->AssignReg(RegisterPool::fa0);
	proc->AddNamedArtefact("n", argVar);
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(7));
	proc->SetStatementSequence(statements);
	proc->setLabel("In_ReadReal");
	procList["In.ReadReal"] = proc;
}