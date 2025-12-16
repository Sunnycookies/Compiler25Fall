#pragma once

#include <iostream>
#include <cassert>

class Operand
{
private:
    union operand
    {
        int reg_no;
        int imm_value;
    } value;
    static int reg_count;
public:
    enum operand_type
    {
        REG,
        IMM,
    } type;
    Operand();
    Operand(const Operand &);
    Operand(const operand_type &, const int & = 0);
    ~Operand();
    int ImmValue();
    bool IsReg();
    friend std::ostream &operator<<(std::ostream &, const Operand &);
};