#pragma once

#include <iostream>
#include <cassert>
#include "koopa.h"

class Backend
{
public:
    Backend(const char *koopa_str);

    ~Backend();

    friend std::ostream &operator<<(std::ostream &os, const Backend &backend);

    void Visit(std::ostream &os) const;

private:
    koopa_raw_program_t raw_program;

    koopa_raw_program_builder_t builder;

    void Visit(std::ostream &os, const koopa_raw_slice_t &slice) const;

    void Visit(std::ostream &os, const koopa_raw_function_t &func) const;

    void Visit(std::ostream &os, const koopa_raw_basic_block_t &bb) const;

    void Visit(std::ostream &os, const koopa_raw_value_t &value) const;

    void Visit(std::ostream &os, const koopa_raw_return_t &ret) const;

    void Visit(std::ostream &os, const koopa_raw_integer_t &integer) const;
};