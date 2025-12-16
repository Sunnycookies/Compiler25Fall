#include "operand.hpp"

int AsOperand::reg_count = 0;

AsOperand::AsOperand()
{
    type = IMM;
    value.imm_value = 0;
}

AsOperand::~AsOperand()
{
    // pass
}

AsOperand::AsOperand(const AsOperand &operant)
{
    type = operant.type;
    value = operant.value;
}

AsOperand::AsOperand(const operant_type &t, const int &v)
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

int AsOperand::ImmValue()
{
    assert(type == IMM);
    return value.imm_value;
}

bool AsOperand::IsReg()
{
    return type == REG;
}

std::ostream &operator<<(std::ostream &os, const AsOperand &operant)
{
    if (operant.type == AsOperand::REG)
    {
        os << "%" << operant.value.reg_no;
    }
    else if (operant.type == AsOperand::IMM)
    {
        os << operant.value.imm_value;
    }
    return os;
}