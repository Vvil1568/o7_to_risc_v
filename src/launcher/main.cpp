#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
// Начальные установки параметров компилятора и его запуск
void Compile(const char* str);

void to_cout(const std::vector<std::string> &v)
{
  std::copy(v.begin(), v.end(),
    std::ostream_iterator<std::string>{std::cout, "\n"});
}

void print_help() {
    std::cout << "Help screen\n";
    std::cout << "Options:\n";
    std::cout << "Use --file fileName to specify the file to compile\n";
}

int main(int argc, const char *argv[])
{
    std::string fileName;
        
    if (argc != 3 || strcmp(argv[1], "--file")!=0) {
        print_help();
        return 0;
    }
    else {
        fileName = argv[2];
        std::cout << "file is: " << fileName << "\n";
    }
    
    // После получения имен открытие и чтение файла с модулем
    // с последующей передачей их на обработку.
    // Пока файл с модулем открывается напрямую.
    //std::string exportName{packageDir + "/export"};
    //std::string artefactName{packageDir + "/smile/" + entityName};
    //std::string rigName{packageDir + "/rig/" + entityName};
    std::string compiledFile{fileName};
    
    // Компилируемый модуль читается из файла в строку
    std::ifstream moduleStream(fileName);
    if(moduleStream.fail()) {
        std::cout << "Module" << fileName << " isn't accesible" << std::endl;
        return 1;
    }

    std::stringstream ss;
    ss << (moduleStream.rdbuf());
    std::string o7module(ss.str()); // Модуль Оберона 7
    /// Тестовый вывод прочитанного артефакта
    std::cout << o7module << std::endl;

    Compile(o7module.c_str());
    
    return 0;
}
