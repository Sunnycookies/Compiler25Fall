#pragma once

#include <iostream>
#include <cassert>
#include <unordered_map>
#include "koopa.h"
#include "defs.hpp"

class Stack
{
private:
    int capacity;
    int size;
    std::unordered_map<koopa_raw_value_t, int> value_off_map;

public:
    Stack();
    Stack(const int &cap);
    bool Find(const koopa_raw_value_t &value);
    void Push(const koopa_raw_value_t &value, const int &type_size);
    int GetOffset(const koopa_raw_value_t &value);
    int GetStackSize();
};