#pragma once

#include <string>
#include <iostream>
#include <deque>
#include "operand.hpp"

#ifdef DEBUG
extern std::ostream &debug;
#endif

const int DATA_SIZE = sizeof(int);
const int ALIGN_SIZE = 16;

const std::string MODE_KOOPA = "-koopa";
const std::string MODE_RISCV = "-riscv";
const std::string MODE_PREF = "-pref";

class BType
{
public:
    enum data_type
    {
        VOID,
        INT,
        ARRAY,
    } type;
    BType *array_base_type;
    int array_size;
    BType();
    BType(const data_type &t, std::deque<Operand> array_sizes = {});
    BType(const BType &t);
    ~BType();
    BType &operator=(const BType &t);
    bool IsVoid() const;
    bool IsInt() const;
    bool IsArray() const;
    std::deque<int> ArraySizes() const;
    bool operator==(const BType &t) const;
    friend std::ostream &operator<<(std::ostream &os, const BType &type);
};