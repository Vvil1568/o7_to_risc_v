#ifndef COMMONDATA_H
#define COMMONDATA_H

#include <vector>
#include <string>
#include "variable.h"
#include "const.h"
#include "statement.h"

class StatementContext;

// ����� ������������ ������, ��������������� ��������� ��� ������ � ��������
class CommonData {
public:
    void SetStatementSequence(std::vector<StatementContext*> statements);
    std::vector<StatementContext*> GetStatementSequence();
    // ���������� ������������ ���������
    void AddNamedArtefact(std::string name, Context* context, bool access = false);
    // ��������� ���� �� ��� ��������
    TypeContext* GetTypeFromName(std::string name);
    // ��������� ��������� �� �� ��������
    ConstContext* GetConstFromName(std::string name);
    // ��������� ���������� �� �� ��������
    VarContext* GetVarFromName(std::string name);
    // ��������� ������ ���� ����������� ����������
    std::vector<NamedArtefact*> getNamedArtefacts() { return namedArtefacts; }
protected:
    std::string name = "";  // ��� ����� ������
    // ������ ���������� � ������������������ �������
    std::vector<NamedArtefact*> reservedNamedArtefacts;
    // ������ ����������� ����������, ����������� � ������
    std::vector<NamedArtefact*> namedArtefacts;
    std::vector<StatementContext*> statements;
};
#endif