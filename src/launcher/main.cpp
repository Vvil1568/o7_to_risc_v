#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
// Начальные установки параметров компилятора и его запуск
std::vector<std::string> Compile(const char* str);

void to_cout(const std::vector<std::string> &v)
{
  std::copy(v.begin(), v.end(),
    std::ostream_iterator<std::string>{std::cout, "\n"});
}

void print_help() {
    std::cout << "Help screen\n";
    std::cout << "You should specify the path to the compiled file as the first argument\n";
    std::cout << "And the resulting file path as the second argument\n";
}
//o7p.exe D:\HSE\oberon7compiler\workspace\o7\big\41.o7 D:\HSE\oberon7compiler\workspace\o7\big\41.s
int main(int argc, const char *argv[])
{
    std::string inputFileName = "D:\\HSE\\oberon7compiler\\workspace\\o7\\big\\gcd.o7", outputFileName = "D:\\HSE\\oberon7compiler\\workspace\\o7\\big\\gcd.s";
    //std::string fileName = "D:\\HSE\\oberon7compiler\\workspace\\o7\\big\\41.o7";
    
    if (argc != 3) {
        print_help();
        return 1;
    }
    else {
        inputFileName = argv[1];
        outputFileName = argv[2];
        std::cout << "Input file is: " << inputFileName << "\n";
        std::cout << "Output file is: " << outputFileName << "\n";
    }
    
    // После получения имен открытие и чтение файла с модулем
    // с последующей передачей их на обработку.
    // Пока файл с модулем открывается напрямую.
    std::string compiledFile{inputFileName};
    
    // Готовим входной поток
    std::ifstream moduleStream(inputFileName);
    if(moduleStream.fail()) {
        std::cout << "Module \"" << inputFileName << "\" isn't accesible" << std::endl;
        return 1;
    }

    // Готовим выходной поток
    std::ofstream outputStream(outputFileName);
    if (outputStream.fail()) {
        std::cout << "Resulting file \"" << outputFileName << "\" isn't accesible" << std::endl;
        return 1;
    }

    std::stringstream ss;
    ss << (moduleStream.rdbuf());
    std::string o7module(ss.str()); // Модуль Оберона 7

    std::vector<std::string> res = Compile(o7module.c_str());
    for (std::string resStr : res) {
        outputStream << resStr << std::endl;
    }
    return 0;
}
