#include "type.h"

// Вывод отладочной информации об общем контексте типа
void TypeContext::debugOut() {
    std::cout << "TYPE ";
    ///debugInfoAboutName();
}

// Вывод отладочной информации о булевском типе
void TypeBoolContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "BOOLEAN, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о целочисленном типе
void TypeIntegerContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "INTEGER, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о действительном типе
void TypeRealContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "REAL, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о символьном типе
void TypeCharContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "CHAR, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о множественном типе
void TypeSetContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "SET, size = " << typeSize; // << std::endl;
}

// Добавление к записи, объявленной в модуле именованного поля
void TypeRecordContext::AddNamedModuleField(std::string name, TypeContext* typeContext, bool access) {
    NamedArtefact tmpNamed{name, typeContext, access};
    namedFields.push_back(tmpNamed);
    typeSize += typeContext->getTypeSize();
}

// Вывод отладочной информации о записи
void TypeRecordContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "RECORD, size = " << typeSize; // << std::endl;
    if(parentContext != nullptr) {
        std::cout << " (";
        parentContext->debugOut();
        std::cout << ")";
    }
    for(NamedArtefact field: namedFields) {
        std::cout << std::endl;
        std::cout << "    " << field.getName() << (field.getAccess()? "*: ":": ");
        field.getContext()->debugOut();
    }
}

// Вывод отладочной информации о типе - указателе
void TypePointerContext::debugOut() {
    TypeContext::debugOut();
    std::cout << "POINTER TO (size = " << typeSize << ") {";
    recordType->debugOut();
    std::cout << "}"; // << std::endl;
}

