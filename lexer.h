#pragma once

#ifndef UNTITLED_LEXER_H
#define UNTITLED_LEXER_H

#include <iostream>
#include <string>
#include <vector>

#include "tokens.h"

// ----------------------------------------------
// LEXER
// ----------------------------------------------
std::vector<std::pair<TOKENS, std::string>> lexer(std::string& line) {
    std::size_t pos = 0;
    std::vector<std::pair<TOKENS, std::string>> vec;

    while (pos < line.size()) {
        if (isspace(line[pos])) {
            ++pos;
        }
        else if (line[pos] == ',') {
            vec.push_back({COMMA, std::string(1, line[pos])});
            ++pos;
        }
        else if (isalpha(line[pos])) {
            std::string buff;
            while (isalpha(line[pos])) {
                buff += line[pos];
                ++pos;
            }
            if (buff == "ah" || buff == "al" || buff == "bh" || buff == "bl" || buff == "ch" || buff == "cl" || buff == "dh" || buff == "dl" ||
                buff == "di" || buff == "si" || buff == "bp" || buff == "sp")
            {
                vec.push_back({REGISTERS, buff});
            }
            else if (buff ==  "mov" || buff == "push" || buff == "pop") {
                vec.push_back({COMMANDS, buff});
            }
            else if (buff == "add" || buff == "sub" || buff == "imul" || buff == "idiv") {
                vec.push_back({OPERATIONS, buff});
            }
            else if (buff == "or" || buff == "and" || buff == "xor" || buff == "cmp" || buff == "not") {
                vec.push_back({LOPERATIONS, buff});
            }
            else if (buff == "int") {
                vec.push_back({INT, buff});
            }
            else throw std::runtime_error("Unexpected word: " + buff);
        }
        else if (isalnum(line[pos])) {
            std::string number;
            while (isalnum(line[pos])) {
                number += line[pos];
                ++pos;
            }
            vec.push_back({NUMS, number});
        }
        else {
            throw std::runtime_error("Unexpected symbol: " + std::string(1, line[pos]));
        }
    }
    return vec;
}

#endif //UNTITLED_LEXER_H
