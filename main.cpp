#include <iostream>
#include <string>

#include "parser.h"

//#define DEBUG

int main(int argc, char* argv[]) {
    std::string buff;

#ifdef DEBUG
    auto print_vector = [](std::vector<std::pair<TOKENS, std::string>>& vec)
    {
        for (const auto& elem : vec) {
            std::cout << "Token: " << elem.first << " Value: " << elem.second << '\n';
        }
    };
#endif

    auto printRegs = []()
    {
        std::cout << "REGISTERS:" << '\n';
        print_regs();
    };
    auto printFlags = []()
    {
        std::cout << "FLAGS:" << '\n';
        print_flags();
    };

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, buff);
        if (buff == "exit") break;
        else if (buff == "registers") printRegs();
        else if (buff == "flags") printFlags();

        try {
            auto tokens = lexer(buff);
            Parser parser(tokens);
            parser.parser();
        }
        catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    return 0;
}