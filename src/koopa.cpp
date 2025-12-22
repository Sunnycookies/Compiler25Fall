#include "koopa.hpp"

KoopaCode::KoopaCode(std::ostream &os)
{
    pos = &os;
}

void KoopaCode::SetOstream(std::ostream &os)
{
    pos = &os;
}

void KoopaCode::EndCurBrac()
{
    *pos << "}\n\n";
}

void KoopaCode::Type(const BType::data_type &type)
{
    switch (type)
    {
    case BType::INT:
        *pos << " i32";
        return;
    
    case BType::VOID:
    default:
        return;
    }
}

void KoopaCode::PreFunc(const std::string &func)
{
    bool flag = true;
    *pos << "fun @" << func << "(";
}

void KoopaCode::FParam(const BType::data_type &type, const std::string &ident, const bool &comma)
{
    *pos << (comma ? ", " : "") << "@" << ident << ":";
    Type(type);
}

void KoopaCode::StoreFParam(const std::string &temp, const std::string &fparam)
{
    *pos << "\tstore @" << fparam << ", %" << temp << "\n";
}

void KoopaCode::PostFunc(const BType::data_type &type)
{
    *pos << ")" << (type == BType::VOID ? "" : ":");
    Type(type);
    *pos << " {\n";
}

void KoopaCode::Label(const std::string &label)
{
    *pos << "%" << label << ":\n";
}

void KoopaCode::Alloc(const std::string &var, const BType::data_type &type, const bool &temp)
{
    *pos << "\t" << (temp ? "%" : "@") << var << " = alloc";
    Type(type);
    *pos << "\n";
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
    *pos << "\t" << dst << " = ge " << src1 << ", " << src2 << "\n";
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

void KoopaCode::Ret(const Operand &reg_or_imm)
{
    if (reg_or_imm.IsNormal())
    {
        *pos << "\tret " << reg_or_imm << "\n";
    }
    else
    {
        *pos << "\tret\n";
    }
}

void KoopaCode::Call(const std::string &func, std::deque<Operand> &params, const Operand &ret)
{
    if (ret.IsNormal())
    {
        *pos << "\t" << ret << " = call @" << func << "(";
    }
    else
    {
        *pos << "\tcall @" << func << "(";
    }
    bool comma = false;
    for (int i = 0, n = params.size(); i < n; ++i)
    {
        if (comma)
        {
            *pos << ", ";
        }
        *pos << params[i];
        comma = true;
    }
    *pos << ")\n";
}