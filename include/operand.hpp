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
    Operand(const Operand &operand);
    Operand(const operand_type &t, const int &v = 0);
    ~Operand();
    int ImmValue();
    bool IsReg();
    bool IsNormal();
    Operand &SetAsReturnMark();
    bool IsReturnMark();
    Operand &SetAsLoopInterruption();
    bool IsLoopInterruption();
    Operand operator !=(const int &v);
    friend std::ostream &operator<<(std::ostream &os, const Operand &operand);
};