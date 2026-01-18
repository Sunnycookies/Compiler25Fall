#include "riscv.hpp"

RiscvCode::RiscvCode(std::ostream &os)
{
    pos = &os;
}

bool RiscvCode::ImmOutOfRange(const int &imm)
{
    return imm < -2048 || imm > 2047;
}

void RiscvCode::SetOstream(std::ostream &os)
{
    pos = &os;
}

void RiscvCode::Data()
{
    *pos << "\n\t.data\n";
}

void RiscvCode::Text()
{
    *pos << "\n\t.text\n";
}

void RiscvCode::Global(const char *name)
{
    *pos << "\t.global " << name << "\n";
}

void RiscvCode::Label(const char *name)
{
    *pos << name << ":\n";
}

void RiscvCode::Word(const int &imm)
{
    *pos << "\t.word " << imm << "\n";
}

void RiscvCode::Zero(const int &data_size)
{
    *pos << "\t.zero " << data_size << "\n";
}

void RiscvCode::Lw(const Register &rs, const Register &rd, const int &imm)
{
    if (ImmOutOfRange(imm))
    {
        Register temp = Register();
        Li(temp, imm);
        Add(temp, temp, rs);
        *pos << "\tlw " << rd << ", (" << temp << ")\n";
    }
    else
    {
        *pos << "\tlw " << rd << ", " << imm << "(" << rs << ")\n";
    }
}

void RiscvCode::Sw(const Register &rd, const Register &rs, const int &imm)
{
    if (ImmOutOfRange(imm))
    {
        Register temp = Register();
        Li(temp, imm);
        Add(temp, temp, rd);
        *pos << "\tsw " << rs << ", (" << temp << ")\n";
    }
    else
    {
        *pos << "\tsw " << rs << ", " << imm << "(" << rd << ")\n";
    }
}

void RiscvCode::Add(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tadd " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Addi(const Register &rd, const Register &rs1, const int &imm)
{
    if (ImmOutOfRange(imm))
    {
        Register temp = Register();
        Li(temp, imm);
        Add(rd, rs1, temp);
    }
    else
    {
        *pos << "\taddi " << rd << ", " << rs1 << ", " << imm << "\n";
    }
}

void RiscvCode::Sub(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tsub " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Slt(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tslt " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Sgt(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tsgt " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Seqz(const Register &rd, const Register &rs)
{
    *pos << "\tseqz " << rd << ", " << rs << "\n";
}

void RiscvCode::Snez(const Register &rd, const Register &rs)
{
    *pos << "\tsnez " << rd << ", " << rs << "\n";
}

void RiscvCode::Sll(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tsll " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Xor(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\txor " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Xori(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\txori " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RiscvCode::Or(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tor " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Ori(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\tori " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RiscvCode::And(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tand " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Andi(const Register &rd, const Register &rs1, const int &imm)
{
    *pos << "\tand " << rd << ", " << rs1 << ", " << imm << "\n";
}

void RiscvCode::Mul(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tmul " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Div(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\tdiv " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Rem(const Register &rd, const Register &rs1, const Register &rs2)
{
    *pos << "\trem " << rd << ", " << rs1 << ", " << rs2 << "\n";
}

void RiscvCode::Mv(const Register &rd, const Register &rs)
{
    *pos << "\tmv " << rd << ", " << rs << "\n";
}

void RiscvCode::Li(const Register &rd, const int &imm)
{
    *pos << "\tli " << rd << ", " << imm << "\n";
}

void RiscvCode::Bnez(const Register &rs, const char *label)
{
    *pos << "\tbnez " << rs << ", " << label << "\n";
}

void RiscvCode::J(const char *label)
{
    *pos << "\tj " << label << "\n";
}

void RiscvCode::Call(const char *name)
{
    *pos << "\tcall " << name << "\n";
}

void RiscvCode::La(const Register &rd, const char *name)
{
    *pos << "\tla " << rd << ", " << name << "\n";
}

void RiscvCode::Ret()
{
    *pos << "\tret\n";
}