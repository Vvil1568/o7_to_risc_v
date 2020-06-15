#include <boost/program_options.hpp>
#include <string>
#include <vector>
//#include <algorithm>
//#include <iterator>
#include <iostream>
#include <fstream>

using namespace boost::program_options;

// Начальные установки параметров компилятора и его запуск
void Compile(const char* str);

void to_cout(const std::vector<std::string> &v)
{
  std::copy(v.begin(), v.end(),
    std::ostream_iterator<std::string>{std::cout, "\n"});
}

int main(int argc, const char *argv[])
{
    // Распознавание опций командной строки, обеспечивающих выбор
    // каталога проекта и компилируемой сущности (артефакта)
    std::string packageDir;     // каталог пакета содержащего компилируемую сущность
    std::string fileName;         // компилируемая сущность

    try {
        options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Help screen")
//            ("package,p", value(&packageDir), "Package")
/*            ("entity,e", value<std::vector<std::string>>()->
                multitoken()->zero_tokens()->composing(), "Compiled entity");
*/
            ("file,f", value(&fileName), "File with module");

        // Ограничение позиционных аргументов (возможны без опции) одним
        positional_options_description pos_desc;
        //pos_desc.add("phone", -1);
        pos_desc.add("file", 1);

        command_line_parser parser{argc, argv};
        parser.options(desc).positional(pos_desc).allow_unregistered();
        parsed_options parsed_options = parser.run();

        variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        // Вывод подсказки
        if (vm.count("help")) {
            if(argc > 2) {
                std::cout << "Use only one option --help or -h for help\n";
                return 0;
            }
            std::cout << desc << '\n';
            return 0;
        }
    
        // Вывод непозиционных параметров (в нашем случае он пока один)
        if (vm.count("file")) {
            /*
            to_cout(vm["entity"].as<std::vector<std::string>>());
            std::cout << "Entity is: " << vm["entity"].as<std::vector<std::string>>()[0] << "\n";
            */
            std::cout << "file is: " << fileName << "\n";
        }

/*
        if (vm.count("package")) {
            std::cout << "Package is: " << packageDir << '\n';
            //return 0;
        }
*/
    }
    catch (const error &ex) {
        std::cerr << ex.what() << '\n';
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
    ss << moduleStream.rdbuf();
    std::string o7module(ss.str()); // Модуль Оберона 7
    /// Тестовый вывод прочитанного артефакта
    std::cout << o7module << std::endl;

    Compile(o7module.c_str());
    
    return 0;
}
