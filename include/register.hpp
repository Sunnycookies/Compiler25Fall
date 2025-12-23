#pragma once

#include <iostream>
#include <cassert>
#include "koopa.h"

class Register
{
private:
    static int usable_reg_num;
    static int ret_reg_no;
    static int sp_no;
    static int ra_no;
    static int *reg_record;
    int reg_no;

public:
    enum special_register_type
    {
        ZERO,
        RET,
        SP,
        RA,
        PARAM,
    };
    static int PARAM_REG_NUM;
    Register(const special_register_type &type = ZERO, const int &id = 0);
    Register(const koopa_raw_value_t &value);
    ~Register();
    void Unallocate();
    friend std::ostream &operator<<(std::ostream &os, const Register &reg);
};