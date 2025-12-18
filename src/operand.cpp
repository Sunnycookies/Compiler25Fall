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

Operand &Operand::SetAsReturnMark()
{
    type = REG;
    value.reg_no = -1;
    return *this;
}

bool Operand::IsReturnMark()
{
    return type == REG && value.reg_no < 0;
}

std::ostream &operator<<(std::ostream &os, const Operand &operand)
{
    if (operand.type == Operand::REG)
    {
        os << "%" << operand.value.reg_no;
    }
    else if (operand.type == Operand::IMM)
    {
        os << operand.value.imm_value;
    }
    return os;
}