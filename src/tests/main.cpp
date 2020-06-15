#include <iostream>
//#include <fstream>
//#include <sstream>

//using namespace std;

#include "module.h"
#include "creator.h"

int main()
{
    /*
    //std::ifstream instream("in.txt");
    if(instream.fail()) {
        std::cout << "File did not open" << std::endl;
        return 1;
    }
    std::stringstream ss;
    ss << instream.rdbuf();
    std::string str(ss.str());
    //std::cout << str << std::endl;

    //InitParser(str.c_str());
   // StartParser();
*/
    std::cout << "Hello model!" << std::endl;

    Module module;
    Creator creator;
    module.setModuleName("Hello");

    ImportContext* iContext = creator.CreateImportContext("M", "AM");
    module.AddNamedArtefact("AM", iContext);
    ///iContext->debugInfoAboutName();

    ConstContext* cContext = creator.CreateConstInt(10);
    module.AddNamedArtefact("Ten", cContext, true);
    cContext = creator.CreateConstReal(3.14);
    module.AddNamedArtefact("pi", cContext);
    cContext = creator.CreateConstBool(true);
    module.AddNamedArtefact("t", cContext);
    cContext = creator.CreateConstString("Hello, World!");
    module.AddNamedArtefact("hello", cContext, true);
    ///cContext->debugInfoAboutName();

    TypeContext* tContext = creator.CreateTypeInt();
    module.AddNamedArtefact("Int", tContext, true);
    tContext = creator.CreateTypeBool();
    module.AddNamedArtefact("Bool", tContext, true);
    tContext = creator.CreateTypeChar();
    module.AddNamedArtefact("Char", tContext);
    tContext = creator.CreateTypeSet();
    module.AddNamedArtefact("Set", tContext, false);

    TypeRecordContext* recordType = creator.CreateTypeRecord();
    tContext = creator.CreateTypeSet();
    recordType->AddNamedModuleField("f1", tContext, true);
    tContext = creator.CreateTypeReal();
    recordType->AddNamedModuleField("f5", tContext, true);
    tContext = creator.CreateTypeBool();
    recordType->AddNamedModuleField("f3", tContext, false);
    module.AddNamedArtefact("Record", recordType);

    TypeRecordContext* recordType2 = creator.CreateTypeRecord(recordType);
    tContext = creator.CreateTypeSet();
    recordType2->AddNamedModuleField("c1", tContext, true);
    tContext = creator.CreateTypeBool();
    recordType2->AddNamedModuleField("c2", tContext, false);
    module.AddNamedArtefact("ChildRecord", recordType2);

    TypePointerContext* pContext = creator.CreateTypePointer(recordType);
    module.AddNamedArtefact("PointerToRecord", pContext, true);

    VarContext* vContext = creator.CreateVariable(creator.CreateTypeReal());
    module.AddNamedArtefact("var1", vContext);
    tContext = creator.CreateTypeReal();
    module.AddNamedArtefact("Real", tContext, true);
    vContext = creator.CreateVariable(tContext);
    module.AddNamedArtefact("var2", vContext, true);

    module.debugOut();
    return 0;
}
