#include "operand.hpp"

int Operand::reg_count = 0;

Operand::Operand()
{
    type = IMM;
    value.imm_value = 0;
}

Operand::~Operand()
{
    // pass
}

Operand::Operand(const Operand &operant)
{
    type = operant.type;
    value = operant.value;
}

Operand::Operand(const operand_type &t, const int &v)
{
    type = t;
    if (type == REG)
    {
        value.reg_no = reg_count++;
    }
    else if (type == IMM)
    {
        value.imm_value = v;
    }
}

int Operand::ImmValue()
{
    assert(type == IMM);
    return value.imm_value;
}

bool Operand::IsReg()
{
    return type == REG;
}

std::ostream &operator<<(std::ostream &os, const Operand &operant)
{
    if (operant.type == Operand::REG)
    {
        os << "%" << operant.value.reg_no;
    }
    else if (operant.type == Operand::IMM)
    {
        os << operant.value.imm_value;
    }
    return os;
}