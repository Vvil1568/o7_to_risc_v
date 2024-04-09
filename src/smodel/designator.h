#ifndef DESIGNATOR_H
#define DESIGNATOR_H

#include "context.h"
#include "registerpool.h"

class ExprContext;
class TypeContext;

// �����, ������������ �������� �����������.
class DesignatorActionContext : public Context {

protected:
    Register* assignedRegister = nullptr;
public:
    // ����� ���������� ���������� � ��������� �����������
    virtual void debugOut() {
        std::cout << "DESIGNATOR";
    }

    virtual void generateAsmCode() = 0;

    virtual void AssignReg(Register* reg) {
        assignedRegister = reg;
    }
};

class IndexDesignActionContext : public DesignatorActionContext {
public:
    IndexDesignActionContext(ExprContext* index) {
        this->index = index;
    }

    void setType(TypeContext* type) { this->type = type; }

    virtual void generateAsmCode();

private:
    TypeContext* type;
    ExprContext* index;
};

class FieldGetDesignActionContext : public DesignatorActionContext {
public:
    FieldGetDesignActionContext(std::string name) {
        this->name = name;
    }

    void setType(TypeContext* type) { this->type = type; }

    std::string getName() { return name; }

    virtual void generateAsmCode();

private:
    TypeContext* type;
    std::string name;
};

class PointerDerefDesignActionContext : public DesignatorActionContext {
public:
    PointerDerefDesignActionContext() { }

    virtual void generateAsmCode();
};
#endif // DESIGNATOR_H