#include "register.hpp"

koopa_raw_value_t *Register::reg_record = new koopa_raw_value_t[Register::usable_reg_num + 1];

std::unordered_map<koopa_raw_value_t, int> Register::allocated_value;

int Register::usable_reg_num = 15;

int Register::ret_reg_no = 8;

int Register::sp_no = 16;

int Register::ra_no = 17;

int Register::PARAM_REG_NUM = 8;

int Register::curr_reg = 1;

void Register::GetReg()
{
    reg_no = curr_reg;
    curr_reg++;
    if (curr_reg > usable_reg_num)
    {
        curr_reg -= usable_reg_num;
    }
    Unallocate();
}

void Register::Initialize()
{
    for (int i = Register::usable_reg_num; i >= 0; --i)
    {
        reg_record[i] = nullptr;
    }
    allocated_value.clear();
    curr_reg = 1;
}

bool Register::Cached(const koopa_raw_value_t &value)
{
    return allocated_value.find(value) != allocated_value.end();
}

Register::Register(const special_register_type &type, const int &id)
{
    switch (type)
    {
    case RET:
        reg_no = ret_reg_no;
        break;
    case SP:
        reg_no = sp_no;
        return;
    case RA:
        reg_no = ra_no;
        return;
    case PARAM:
        reg_no = ret_reg_no + id;
        break;
    case IMM:
        GetReg();
        return;
    default:
    case ZERO:
        reg_no = 0;
        return;
    }
    Unallocate();
}

Register::Register(const koopa_raw_value_t &value, bool *cached)
{
    bool is_cached = allocated_value.find(value) != allocated_value.end();
    if (is_cached)
    {
        reg_no = allocated_value[value];
    }
    else
    {
        GetReg();
        reg_record[reg_no] = value;
        allocated_value[value] = reg_no;
    }
    if (cached)
    {
        *cached = is_cached;
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
    if (reg_record[reg_no])
    {
        allocated_value.erase(reg_record[reg_no]);
        reg_record[reg_no] = nullptr;
    }
}