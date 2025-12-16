#pragma once

#include <string>
#include <iostream>

#ifdef DEBUG
extern std::ostream &debug;
#endif

const std::string MODE_KOOPA = "-koopa";
const std::string MODE_RISCV = "-riscv";
const std::string MODE_PREF = "-pref";