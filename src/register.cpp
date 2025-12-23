#include "register.hpp"

int Register::usable_reg_num = 15;

int Register::ret_reg_no = 8;

int Register::sp_no = 16;

int Register::ra_no = 17;

int *Register::reg_record = new int[Register::usable_reg_num + 1];

int Register::PARAM_REG_NUM = 8;

Register::Register(const special_register_type &type, const int &id)
{
    switch (type)
    {
    case RET:
        assert(!reg_record[ret_reg_no]);
        reg_no = ret_reg_no;
        reg_record[reg_no] = 1;
        return;
    case SP:
        reg_no = sp_no;
        return;
    case RA:
        reg_no = ra_no;
        return;
    case PARAM:
        reg_no = ret_reg_no + id;
        assert(reg_record[reg_no] == 0);
        reg_record[reg_no] = 1;
        return;
    default:
    case ZERO:
        reg_no = 0;
        return;
    }
}

Register::Register(const koopa_raw_value_t &value)
{
    for (int i = 1; i <= usable_reg_num; ++i)
    {
        if (reg_record[i] == 0)
        {
            reg_record[i] = 1;
            reg_no = i;
            return;
        }
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
    else if (reg.reg_no == Register::sp_no)
    {
        os << "sp";
    }
    else if (reg.reg_no == Register::ra_no)
    {
        os << "ra";
    }
    else if (reg.reg_no < Register::ret_reg_no)
    {
        os << "t" << reg.reg_no - 1;
    }
    else if (reg.reg_no <= Register::usable_reg_num)
    {
        os << "a" << reg.reg_no - Register::ret_reg_no;
    }
    return os;
}

void Register::Unallocate()
{
    reg_record[reg_no] = 0;
}