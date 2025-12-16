#pragma once

#include <iostream>
#include <cassert>

class AsOperand
{
private:
    union operant
    {
        int reg_no;
        int imm_value;
    } value;
public:
    enum operant_type
    {
        REG,
        IMM,
    } type;
    static int reg_count;
    AsOperand();
    AsOperand(const AsOperand &);
    AsOperand(const operant_type &, const int & = 0);
    ~AsOperand();
    int ImmValue();
    bool IsReg();
    friend std::ostream &operator<<(std::ostream &, const AsOperand &);
};