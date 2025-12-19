#pragma once

#include <iostream>
#include <string>
#include "register.hpp"

class RISCCode
{
private:
    std::ostream *pos;

public:
    RISCCode(std::ostream &os = std::cout);
    void SetOstream(std::ostream &os);
    void Text();
    void Global(const char *name);
    void Label(const char *name);
    void Lw(const Register &rd, const Register &rs, const int &imm);
    void Sw(const Register &rs1, const Register &rs2, const int &imm);
    void Add(const Register &rd, const Register &rs1, const Register &rs2);
    void Addi(const Register &rd, const Register &rs1, const int &imm);
    void Sub(const Register &rd, const Register &rs1, const Register &rs2);
    void Slt(const Register &rd, const Register &rs1, const Register &rs2);
    void Sgt(const Register &rd, const Register &rs1, const Register &rs2);
    void Seqz(const Register &rd, const Register &rs);
    void Snez(const Register &rd, const Register &rs);
    void Xor(const Register &rd, const Register &rs1, const Register &rs2);
    void Xori(const Register &rd, const Register &rs1, const int &imm);
    void Or(const Register &rd, const Register &rs1, const Register &rs2);
    void Ori(const Register &rd, const Register &rs1, const int &imm);
    void And(const Register &rd, const Register &rs1, const Register &rs2);
    void Andi(const Register &rd, const Register &rs1, const int &imm);
    void Mul(const Register &rd, const Register &rs1, const Register &rs2);
    void Div(const Register &rd, const Register &rs1, const Register &rs2);
    void Rem(const Register &rd, const Register &rs1, const Register &rs2);
    void Mv(const Register &rd, const Register &rs);
    void Li(const Register &rd, const int &imm);
    void Bnez(const Register &rs, const char *label);
    void J(const char *label);
    void Ret();
};