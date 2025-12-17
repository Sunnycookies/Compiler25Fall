#pragma once

#include <iostream>
#include <cassert>
#include "koopa.h"
#include "register.hpp"
#include "riscv.hpp"
#include "stack.hpp"
#include "defs.hpp"

class Backend
{
public:
    Backend(const char *koopa_str);
    ~Backend();
    friend std::ostream &operator<<(std::ostream &os, Backend &backend);
    void Visit(std::ostream &os);

private:
    koopa_raw_program_t raw_program;
    koopa_raw_program_builder_t builder;
    Stack stack;
    RISCCode printer;
    void Visit(const koopa_raw_slice_t &slice);
    void Visit(const koopa_raw_function_t &func);
    void Visit(const koopa_raw_basic_block_t &bb);
    void Visit(const koopa_raw_return_t &ret);
    void Visit(const koopa_raw_value_t &value);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_load_t &load);
    void Visit(const koopa_raw_store_t &store);
};