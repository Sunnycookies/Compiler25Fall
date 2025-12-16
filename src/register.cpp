#include "register.hpp"

int Register::usable_reg_num = 15;

int Register::ret_reg_no = 8;

int *Register::reg_record = new int[Register::usable_reg_num + 1];

std::unordered_map<koopa_raw_value_t, int> Register::value_reg_map;

Register::Register()
{
    reg_no = 0;
}

Register::Register(const koopa_raw_value_t &value, const bool &allocation_checked)
{
    if (allocation_checked || allocated(value))
    {
        reg_no = value_reg_map[value];
    }
    else
    {
        reg_no = valid_reg_no();
        value_reg_map[value] = reg_no;
    }
}

Register::~Register()
{
    // pass
}

std::ostream &operator<<(std::ostream &os, const Register &reg)
{
    if (reg.reg_no == 0)
    {
        os << "x0";
    }
    else if (reg.reg_no <= 7)
    {
        os << "t" << reg.reg_no - 1;
    }
    else if (reg.reg_no <= 15)
    {
        os << "a" << reg.reg_no - 8;
    }
    return os;
}

bool Register::allocated(const koopa_raw_value_t &value)
{
    return Register::value_reg_map.find(value) != Register::value_reg_map.end();
}

int Register::valid_reg_no()
{
    for (int i = 1; i <= usable_reg_num; ++i)
    {
        if (reg_record[i] == 0)
        {
            reg_record[i] = 1;
            return i;
        }
    }
    assert(false);
    return -1;
}

bool Register::occupy_ret_reg()
{
    return reg_no == ret_reg_no;
}

void Register::unallocate()
{
    reg_record[reg_no] = 0;
}