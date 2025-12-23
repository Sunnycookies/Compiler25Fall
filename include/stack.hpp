#pragma once

#include <iostream>
#include <cassert>
#include <unordered_map>
#include "koopa.h"
#include "defs.hpp"

class Stack
{
private:
    int total_capacity;
    int ra_capacity;
    int local_capacity;
    int temp_capacity;
    int local_offset;
    int temp_offset;
    typedef std::unordered_map<koopa_raw_value_t, int> offset_map_t;
    offset_map_t local_off_map;
    offset_map_t temp_off_map;

public:
    Stack();
    Stack(const int &total_cap, const int &temp_cap, const int &ra_cap);
    bool Find(const koopa_raw_value_t &value);
    void Push(const koopa_raw_value_t &value, const int &type_size, const bool &temp = false);
    void Pop(const int &type_size, const bool &temp = false);
    int GetOffset(const koopa_raw_value_t &value, const bool &temp = false);
    int GetStackSize();
    bool SaveRa();
    int RaOffset();
};