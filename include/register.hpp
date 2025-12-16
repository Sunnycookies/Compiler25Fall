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
    static int *reg_record;
    static std::unordered_map<koopa_raw_value_t, int> value_reg_map;
    static int valid_reg_no();
    int reg_no;

public:
    static bool allocated(const koopa_raw_value_t &value);
    Register();
    Register(const koopa_raw_value_t &, const bool & = false);
    ~Register();
    bool occupy_ret_reg();
    void unallocate();
    friend std::ostream &operator<<(std::ostream &, const Register &);
};