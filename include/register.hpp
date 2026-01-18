#pragma once

#include <iostream>
#include <cassert>
#include "koopa.h"
#include "unordered_map"

class Register
{
private:
    static koopa_raw_value_t *reg_record;
    static std::unordered_map<koopa_raw_value_t, int> allocated_value;
    static int usable_reg_num;
    static int ret_reg_no;
    static int sp_no;
    static int ra_no;
    static int curr_reg;
    int reg_no;
    void GetReg();
    void Unallocate();
 
public:
    enum special_register_type
    {
        ZERO,
        RET,
        SP,
        RA,
        PARAM,
        IMM,
    };
    static int PARAM_REG_NUM;
    static void Initialize();
    static bool Cached(const koopa_raw_value_t &value);
    Register(const special_register_type &type = ZERO, const int &id = 0);
    Register(const koopa_raw_value_t &value, bool *cached = nullptr);
    ~Register();
    friend std::ostream &operator<<(std::ostream &os, const Register &reg);
};