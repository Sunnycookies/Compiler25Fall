#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include "koopa.h"
#include "register.hpp"
#include "riscv.hpp"
#include "stack.hpp"
#include "defs.hpp"

class Backend
{
public:
    static bool pass_entry;
    Backend(const char *koopa_str);
    ~Backend();
    friend std::ostream &operator<<(std::ostream &os, Backend &backend);
    void Visit(std::ostream &os);

private:
    koopa_raw_program_t raw_program;
    koopa_raw_program_builder_t builder;
    Stack stack;
    RiscvCode printer;
    int GetAllocSize(const koopa_raw_type_t &ty);
    int IsPowOfTwo(const int &number);
    void Visit(const koopa_raw_slice_t &slice);
    void Visit(const koopa_raw_function_t &func);
    void Visit(const koopa_raw_basic_block_t &bb);
    void Visit(const koopa_raw_return_t &ret);
    void Visit(const koopa_raw_value_t &value);
    void Visit(const koopa_raw_store_t &store);
    void Visit(const koopa_raw_branch_t &branch);
    void Visit(const koopa_raw_jump_t &jump);
    void Visit(const koopa_raw_aggregate_t &aggregate);
    void Alloc(const koopa_raw_value_t &alloc);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_load_t &load);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_call_t &call);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_global_alloc_t &global_alloc);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_get_ptr_t &get_ptr);
    void Visit(const koopa_raw_value_t &value, const koopa_raw_get_elem_ptr_t &get_elem_ptr);
};