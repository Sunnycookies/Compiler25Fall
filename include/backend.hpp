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
    Register(const koopa_raw_value_t &, const bool &);
    ~Register();
    bool occupy_ret_reg();
    void unallocate();
    friend std::ostream &operator<<(std::ostream &, const Register &);
};

class Backend
{
public:
    Backend(const char *koopa_str);
    ~Backend();
    friend std::ostream &operator<<(std::ostream &, Backend &);
    void Visit(std::ostream &);

private:
    koopa_raw_program_t raw_program;
    koopa_raw_program_builder_t builder;
    std::ostream *pos;
    Register Visit(const koopa_raw_slice_t &);
    Register Visit(const koopa_raw_function_t &);
    Register Visit(const koopa_raw_basic_block_t &);
    Register Visit(const koopa_raw_return_t &);
    Register Visit(const koopa_raw_value_t &);
    Register Visit(const koopa_raw_value_t &, const koopa_raw_integer_t &);
    Register Visit(const koopa_raw_value_t &, const koopa_raw_binary_t &);
};