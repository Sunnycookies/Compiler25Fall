#include "riscv.hpp"

RISCCode::RISCCode(std::ostream &os)
{
    pos = &os;
}

void RISCCode::SetOstream(std::ostream &os)
{
    pos = &os;
}

void RISCCode::Text()
{
    *pos << "\t.text\n";
}

void RISCCode::Global(const char *name)
{
    *pos << "\t.global " << name << "\n";
}

void RISCCode::Label(const char *name)
{
    *pos << name << ":\n";
}

void RISCCode::Lw(const Register &rd, const Register &rs, const int &imm)
{
    *pos << "\tlw " << rs << ", " << imm << "(" << rd << ")\n";
}

void RISCCode::Sw(const Register &rs1, const Register &rs2, const int &imm)
{
    *pos << "\tsw " << rs2 << ", " << imm << "(" << rs1 << ")\n";
}

void RISCCode::Add(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tadd " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Addi(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\taddi " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RISCCode::Sub(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tsub " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Slt(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tslt " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Sgt(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tsgt " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Seqz(const Register &rd, const Register &rs)
{
    *pos << "\tseqz " << rd << ", " << rs << "\n";
}

void RISCCode::Snez(const Register &rd, const Register &rs)
{
    *pos << "\tsnez " << rd << ", " << rs << "\n";
}

void RISCCode::Xor(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\txor " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Xori(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\txori " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RISCCode::Or(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tor " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Ori(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\tori " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RISCCode::And(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tand " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Andi(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\tand " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RISCCode::Mul(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tmul " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Div(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tdiv " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Rem(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\trem " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RISCCode::Mv(const Register &rd, const Register &rs)
{
    *pos << "\tmv " << rd << ", " << rs << "\n";
}

void RISCCode::Li(const Register &rd, const int &imm)
{
    *pos << "\tli " << rd << ", " << imm << "\n";
}

void RISCCode::Ret()
{
    *pos << "\tret\n";
}