#pragma once

#include <iostream>
#include <cassert>
#include "koopa.h"
#include "register.hpp"

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