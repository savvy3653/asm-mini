#pragma once

#ifndef UNTITLED_TOKENS_H
#define UNTITLED_TOKENS_H

#include <string>
#include <map>
#include <stack>
#include <cstdint>

enum TOKENS {
    REGISTERS = 1,
    COMMANDS = 2,
    OPERATIONS = 3,
    LOPERATIONS = 4,
    NUMS = 5,
    INT = 6,
    COMMA = 7
};

std::map<std::string, int8_t> registers {
        {"ah", 0}, {"al", 0},
        {"bh", 0}, {"bl", 0},
        {"ch", 0}, {"cl", 0},
        {"dh", 0}, {"dl", 0},
        {"di", 0}, {"si", 0},
        {"bp", 0}, {"sp", 0}
};

std::stack<int8_t> stack;

std::map<std::string, bool> flags {
        {"CF", 0}, {"ZF", 0},
        {"SF", 0}, {"OF", 0},
        {"PF", 0}, {"AF", 0}
};

#endif //UNTITLED_TOKENS_H
