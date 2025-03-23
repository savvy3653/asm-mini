#pragma once

#ifndef UNTITLED_PARSER_H
#define UNTITLED_PARSER_H

#include <memory>

#include "lexer.h"

int8_t get_register_value(const std::string& reg) {
    return registers[reg];
}

void set_register_value(const std::string& reg, int8_t value) {
    registers[reg] = value;
}

// ----------------------------------------------
// PARSER
// ----------------------------------------------

class Parser {
public:
    Parser(std::vector<std::pair<TOKENS, std::string>>& vector) : vec(vector)
    {}

    void parser() {
        int pos = 0;
        while (pos < vec.size()) {
            if (vec[pos].first == COMMANDS) {
                pos = handleCommands(pos);
            }
            else if (vec[pos].first == OPERATIONS) {
                pos = handleOperations(pos);
            }
            else if (vec[pos].first == LOPERATIONS) {
                pos = handleLOperations(pos);
            }
            else {
                throw std::runtime_error("Unexpected token.");
            }
        }
    }

private:
    std::vector<std::pair<TOKENS, std::string>>& vec;

    static void handleFlags(int res, int value1 = 0, int value2 = 0) {
        if (res > 127) {
            flags["OF"] = 1;
            flags["CF"] = 1;
        } else {
            flags["OF"] = 0;
            flags["CF"] = 0;
        }

        if (res == 0) {
            flags["ZF"] = 1;
        } else {
            flags["ZF"] = 0;
        }

        if (res < 0) {
            flags["SF"] = 1;
        } else {
            flags["SF"] = 0;
        }

        if (res < -128) {
            flags["OF"] = 1;
        } else {
            flags["OF"] = 0;
        }

        flags["AF"] = ((value1 & 0x0F) + (value2 & 0x0F)) > 0x0F;
    }

    void handleValues(int* value1, int* value2, int pos) {
        // error handling
        if (!value1 | !value2) {
            throw std::runtime_error("Null pointer error!");
        }
        if (pos + 2 >= vec.size()) {
            throw std::runtime_error("Invalid operands.");
        }

        if (vec[pos].first == REGISTERS) {
            *value1 = get_register_value(vec[pos].second);
        } else throw std::runtime_error("Unexpected operand.");

        try {
            const std::string& num_str = vec[pos + 2].second;

            if (vec[pos + 2].first == REGISTERS) {
                *value2 = get_register_value(num_str);
            }
            else if (vec[pos + 2].first == NUMS) {
                // number parser
                if (num_str.size() >= 2) {
                    const std::string prefix = num_str.substr(0, 2);
                    if (prefix == "0x") {
                        *value2 = std::stoi(num_str.substr(2), nullptr, 16);
                    }
                    else if (prefix == "0b") {
                        *value2 = std::stoi(num_str.substr(2), nullptr, 2);
                    }
                    else {
                        *value2 = std::stoi(num_str);
                    }
                } else {
                    *value2 = std::stoi(num_str);
                }
            }
            else {
                throw std::runtime_error("Unexpected second operand type");
            }
        }
        catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid number format: " + vec[pos + 2].second);
        }
        catch (const std::out_of_range& e) {
            throw std::runtime_error("Number out of range: " + vec[pos + 2].second);
        }
    }

// --------------------------
// LOGICAL OPERATIONS HANDLING
// --------------------------
    int handleLOperations(int pos) {
        std::string loperation = vec[pos].second;
        ++pos;

        if (loperation == "or") {
            return handleOr(pos);
        }
        else if (loperation == "and") {
            return handleAnd(pos);
        }
        else if (loperation == "xor") {
            return handleXor(pos);
        }
        else if (loperation == "cmp") {
            return handleCmp(pos);
        }
        else if (loperation == "not") {
            return handleNot(pos);
        }
        else {
            throw std::runtime_error("Unknown operation: " + loperation + ".");
        }
    }

    int handleOr(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'OR' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 | value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

    int handleAnd(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'AND' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 & value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

    int handleXor(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'XOR' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 ^ value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

    int handleCmp(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'CMP' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        bool res = value1 == value2;
        if (res) {
            flags["ZF"] = 1;
        } else {
            flags["ZF"] = 0;
        }
        return pos + 3;
    }

    int handleNot(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'NOT' operands.");

        int value1;
        if (vec[pos].first == REGISTERS) {
            value1 = get_register_value(vec[pos].second);
        } else throw std::runtime_error("Unexpected operand.");

        int res = ~value1;
        flags["ZF"] = (value1 == 0);
        flags["SF"] = (value1 < 0);

        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

// --------------------------
// OPERATIONS HANDLING
// --------------------------
    int handleOperations(int pos) {
        std::string operation = vec[pos].second;
        ++pos;

        if (operation == "add") {
            return handleAdd(pos);
        }
        else if (operation == "sub") {
            return handleSub(pos);
        }
        else if (operation == "imul") {
            return handleImul(pos);
        }
        else if (operation == "idiv") {
            return handleIdiv(pos);
        }
        else {
            throw std::runtime_error("Unknown operation: " + operation + ".");
        }
    }

    int handleImul(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'IMUL' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 * value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

    int handleIdiv(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'IDIV' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 / value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

    int handleAdd(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'ADD' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 + value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

    int handleSub(int pos) {
        if (pos >= vec.size()) throw std::runtime_error("Invalid 'SUB' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        int value1, value2;
        handleValues(&value1, &value2, pos);

        int res = value1 - value2;
        handleFlags(res, value1, value2);
        set_register_value(vec[pos].second, static_cast<int8_t>(res));

        return pos + 3;
    }

// --------------------------
// COMMANDS HANDLING
// --------------------------
    int handleCommands(int pos) {
        std::string cmd = vec[pos].second;
        ++pos;

        if (cmd == "mov") {
            return handleMov(pos);
        }
        else if (cmd == "push") {
            return handlePush(pos);
        }
        else if (cmd == "pop") {
            return handlePop(pos);
        }
        else {
            throw std::runtime_error("Unknown command: " + cmd + ".");
        }
    }

    int handleMov(int pos) {
        if (pos + 1 >= vec.size()) throw std::runtime_error("Invalid 'MOV' operands.");
        if (vec[pos].first != REGISTERS) throw std::runtime_error("Invalid 'MOV' operands.");
        if (vec[pos + 1].first != COMMA) throw std::runtime_error("COMMA missed.");

        std::string dest_register = vec[pos].second;
        if (vec[pos + 2].first == NUMS) {
            int8_t value;

            // checking for hex-numbers
            if (vec[pos + 2].second[0] == '0' && vec[pos + 2].second[1] == 'x') {
                value = static_cast<int8_t>(std::stoi(vec[pos + 2].second, nullptr, 16));
            }
            // checking for binary numbers
            else if (vec[pos + 2].second[0] == '0' && vec[pos + 2].second[1] == 'b') {
                std::string num = "";
                std::string initial = vec[pos + 2].second;
                for (int i = 2; i < initial.size(); i++) {
                    num += initial[i];
                }
                value = (std::stoi(num, nullptr, 2));
            }
            // simple number
            else {
                value = static_cast<int8_t>(std::stoi(vec[pos + 2].second));
            }

            set_register_value(dest_register, value);
        }
        else if (vec[pos + 2].first == REGISTERS) {
            int8_t value = get_register_value(vec[pos + 2].second);
            set_register_value(dest_register, value);
        }
        else {
            throw std::runtime_error("Invalid 'MOV' operands.");
        }
        return pos + 3;
    }

    int handlePush(int pos) {
        if (pos >= vec.size())
            throw std::runtime_error("Invalid push operand.");

        if (vec[pos].first == NUMS) {
            auto value = static_cast<int8_t>(std::stoi(vec[pos].second));
            stack.push(value);
        }
        else if (vec[pos].first == REGISTERS) {
            int8_t value = get_register_value(vec[pos].second);
            stack.push(value);
        }
        else {
            throw std::runtime_error("Invalid 'PUSH' operands.");
        }
        return pos + 1;
    };

    int handlePop(int pos) {
        if (pos >= vec.size()) {
            throw std::runtime_error("Invalid pop operand.");
        }
        else if (stack.empty()) {
            throw std::runtime_error("Stack is empty!");
        }

        if (vec[pos].first == REGISTERS) {
            int8_t value = stack.top();
            stack.pop();
            set_register_value(vec[pos].second, value);
        }
        else {
            throw std::runtime_error("Invalid 'POP' operands.");
        }
        return pos + 1;
    }
};

void print_regs() {
    for (const auto& reg : registers) {
        std::cout << reg.first << ": " << (int)reg.second << "\n";
    }
}

void print_flags() {
    for (const auto& reg : flags) {
        std::cout << reg.first << ": " << (int)reg.second << "\n";
    }
}

#endif //UNTITLED_PARSER_H
