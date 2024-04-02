#include "stdprocedures.h"
#include "creator.h"

STDProcedures::STDProcedures() {
	Creator creator;
	ProcContext* proc = new ProcContext();
	proc->setProcedureName("WriteInt");
	proc->AddNamedArtefact("n", creator.CreateArgVariable("n", creator.CreateTypeInt(), false));
	std::vector<StatementContext*> statements;
	statements.push_back(creator.CreateSysCallStatement(1));
	proc->SetStatementSequence(statements);
	proc->setLabel("Out_WriteInt");
	procList["Out.WriteInt"] = proc;

	proc = new ProcContext();
	proc->setProcedureName("WriteStr");
	proc->AddNamedArtefact("str", creator.CreateArgVariable("str", creator.CreateTypeArray(creator.CreateTypeChar(),0), false));
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(4));
	proc->SetStatementSequence(statements);
	proc->setLabel("Out_WriteStr");
	procList["Out.WriteStr"] = proc;

	proc = new ProcContext();
	proc->setProcedureName("ReadInt");
	proc->AddNamedArtefact("n", creator.CreateArgVariable("n", creator.CreateTypeInt(), true));
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(5));
	proc->SetStatementSequence(statements);
	proc->setLabel("In_ReadInt");
	procList["In.ReadInt"] = proc;

	proc = new ProcContext();
	proc->setProcedureName("ReadStr");
	proc->AddNamedArtefact("str", creator.CreateArgVariable("str", creator.CreateTypeArray(creator.CreateTypeChar(), 0), true));
	statements.clear();
	statements.push_back(creator.CreateSysCallStatement(5));
	proc->SetStatementSequence(statements);
	proc->setLabel("In_ReadStr");
	procList["In.ReadStr"] = proc;
}