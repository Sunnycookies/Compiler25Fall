#include "koopa.hpp"

KoopaCode::KoopaCode(std::ostream &os)
{
    pos = &os;
}

void KoopaCode::SetOstream(std::ostream &os)
{
    pos = &os;
}

void KoopaCode::NewLine()
{
    *pos << "\n";
}

void KoopaCode::FrontCurBrac()
{
    *pos << "{\n";
}

void KoopaCode::EndCurBrac()
{
    *pos << "}\n";
}

void KoopaCode::Int()
{
    *pos << "i32";
}

// void KoopaCode::Func(const std::string &func, const std::string &ret_type)
// {
//     *pos << "func @" << func << "(): " << ret_type << "\n";
// }

void KoopaCode::Label(const std::string &label)
{
    *pos << "%" << label << ":\n";
}

void KoopaCode::Alloc(const std::string &var, const bool &temp)
{
    *pos << "\t" << (temp ? "%" : "@") << var << " = alloc ";
}

void KoopaCode::Store(const Operand &reg_or_imm, const std::string &var, const bool &temp)
{
    *pos << "\tstore " << reg_or_imm << ", " << (temp ? "%" : "@") << var << "\n";
}

void KoopaCode::Load(const Operand &reg, const std::string &var, const bool &temp)
{
    *pos << "\t" << reg << " = load " << (temp ? "%" : "@") << var << "\n";
}

void KoopaCode::Br(const Operand &cond, const std::string &t_label, const std::string &f_label)
{
    *pos << "\tbr " << cond << ", %" << t_label << ", %" << f_label << "\n";
}

void KoopaCode::Jump(const std::string &label)
{
    *pos << "\tjump %" << label << "\n";
}

void KoopaCode::Add(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = add " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Sub(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = sub " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Mul(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = mul " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Div(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = div " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Mod(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = mod " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Lt(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = lt " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Gt(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = gt " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Le(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = le " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Ge(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = gr " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Eq(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = eq " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Ne(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = ne " << src1 << ", " << src2 << "\n";
}

void KoopaCode::And(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = and " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Or(const Operand &dst, const Operand &src1, const Operand &src2)
{
    *pos << "\t" << dst << " = or " << src1 << ", " << src2 << "\n";
}

void KoopaCode::Ret()
{
    *pos << "\tret\n";
}

void KoopaCode::RetV(const Operand &reg_or_imm)
{
    *pos << "\tret " << reg_or_imm << "\n";
}