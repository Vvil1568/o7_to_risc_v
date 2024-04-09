#include "designator.h"

#include <string>
#include "expr.h"
#include "type.h"

void IndexDesignActionContext::generateAsmCode() {
	index->generateAsmCode();
	Register* reg = RegisterPool::getInstance().takeFirstFreeReg(RegType::T);
	int elemSize = static_cast<TypeArrayContext*>(type)->getElemType()->getTypeSize();
	CodeGenContext::addCodeComment("Loading single array element size into " + reg->getName());
	CodeGenContext::addCodeLine("li " + reg->getName() + " " + std::to_string(elemSize));
	CodeGenContext::addCodeComment("Calculating the offset between the beginning and the required array slot");
	CodeGenContext::addCodeLine("mul " + reg->getName()+" " + index->getAssignedReg()->getName() + " " + reg->getName());
	index->FreeReg();
	CodeGenContext::addCodeComment("Shifting the pointer to the required array slot");
	CodeGenContext::addCodeLine("add " + assignedRegister->getName() + " " + assignedRegister->getName() + " " + reg->getName());
	RegisterPool::getInstance().freeRegister(reg);
}

void FieldGetDesignActionContext::generateAsmCode() {
	int offset = static_cast<TypeRecordContext*>(type)->getFieldOffset(name);
	CodeGenContext::addCodeComment("Shifting the pointer by the offset from the beginning of the record to the field");
	CodeGenContext::addCodeLine("addi " + assignedRegister->getName() + " " + assignedRegister->getName() + " " + std::to_string(offset));
}

void PointerDerefDesignActionContext::generateAsmCode() {
	CodeGenContext::addCodeComment("Shifting the pointer to the contents of the pointer");
	CodeGenContext::addCodeLine("lw " + assignedRegister->getName() + " " + assignedRegister->getName());
}