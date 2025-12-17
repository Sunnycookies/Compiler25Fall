#pragma once

#include <string>
#include <iostream>

#ifdef DEBUG
extern std::ostream &debug;
#endif

const int DATA_SIZE = sizeof(int);
const int ALIGN_SIZE = 16;

const std::string MODE_KOOPA = "-koopa";
const std::string MODE_RISCV = "-riscv";
const std::string MODE_PREF = "-pref";