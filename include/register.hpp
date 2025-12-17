#pragma once

#include <iostream>
#include <cassert>
#include <unordered_map>
#include "koopa.h"

class Register
{
private:
    static int usable_reg_num;
    static int ret_reg_no;
    static int sp_no;
    static int *reg_record;
    static std::unordered_map<koopa_raw_value_t, int> value_reg_map;
    int reg_no;

public:
    enum special_register_type
    {
        ZERO,
        RET,
        SP,
    };
    Register(const special_register_type &type = ZERO);
    Register(const koopa_raw_value_t &value);
    ~Register();
    void Unallocate();
    friend std::ostream &operator<<(std::ostream &os, const Register &reg);
};