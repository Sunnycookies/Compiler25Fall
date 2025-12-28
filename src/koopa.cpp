#include "koopa.hpp"

void KoopaCode::PrintArray(const std::deque<int> &arr_sizes, const std::deque<Operand> &init_vals, int &index, const int &dim)
{
    *pos << "{";
    if (dim == arr_sizes.size() - 1)
    {
        for (int i = 0; i < arr_sizes[dim]; ++i)
        {
            *pos << init_vals[index++];
            if (i != arr_sizes[dim] - 1)
            {
                *pos << ", ";
            }
        }
    }
    else
    {
        for (int i = 0; i < arr_sizes[dim]; ++i)
        {
            PrintArray(arr_sizes, init_vals, index, dim + 1);
            if (i != arr_sizes[dim] - 1)
            {
                *pos << ", ";
            }
        }
    }
    *pos << "}";
}

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
    *pos << "}\n";
}

void KoopaCode::PreFunc(const std::string &func)
{
    *pos << "\nfun @" << func << "(";
}

void KoopaCode::FParam(const BType &type, const std::string &ident, const bool &comma)
{
    *pos << (comma ? ", " : "") << "@" << ident << ": " << type;
}

void KoopaCode::StoreFParam(const std::string &temp, const std::string &fparam)
{
    *pos << "\tstore @" << fparam << ", @" << temp << "\n";
}

void KoopaCode::PostFunc(const BType &type)
{
    *pos << ")" << (type.IsVoid() ? "" : ": ") << type << " {\n";
}

void KoopaCode::Label(const std::string &label)
{
    *pos << "%" << label << ":\n";
}

void KoopaCode::DeclFunc(const std::string &ident, const std::deque<BType> &ret_and_params)
{
    *pos << "decl @" << ident << "(";
    bool comma = false;
    const BType &return_type = ret_and_params[0];
    for (int i = 1, n = ret_and_params.size(); i < n; ++i)
    {
        if (comma)
        {
            *pos << ", ";
        }
        *pos << ret_and_params[i];
        comma = true;
    }
    *pos << ")" << (return_type.IsVoid() ? "" : ": ") << return_type << "\n";
}

void KoopaCode::Alloc(const std::string &var, const BType &type, const bool &temp)
{
    *pos << "\t" << (temp ? "%" : "@") << var << " = alloc " << type << "\n";
}

void KoopaCode::Alloc(const std::string &arr, const BType &type, const std::deque<Operand> &init_vals)
{
    *pos << "\t@" << arr << " = alloc " << type << "\n";
    if (init_vals.empty())
    {
        return;
    }
    std::deque<std::pair<Operand, int>> elemptrs;
    std::deque<int> arr_sizes = type.ArraySizes();
    int index = 0;
    for (int i = 0; i < arr_sizes[0]; ++i)
    {
        Operand elemptr = Operand(Operand::REG);
        GetElemptr(elemptr, arr, Operand(Operand::IMM, i));
        elemptrs.push_back({elemptr, 1});
    }
    while (!elemptrs.empty())
    {
        auto [current_ptr, dim] = elemptrs.front();
        elemptrs.pop_front();
        if (dim == arr_sizes.size())
        {
            Store(init_vals[index++], current_ptr);
            continue;
        }
        for (int i = 0; i < arr_sizes[dim]; ++i)
        {
            Operand elemptr = Operand(Operand::REG);
            GetElemptr(elemptr, current_ptr, Operand(Operand::IMM, i));
            elemptrs.push_back({elemptr, dim + 1});
        }
    }
}

void KoopaCode::GlobalAlloc(const std::string &var, const BType &type, const Operand &v)
{
    *pos << "\nglobal @" << var << " = alloc " << type << ", ";
    if (v.IsNormal())
    {
        *pos << v << "\n";
    }
    else
    {
        *pos << "zeroinit\n";
    }
}

void KoopaCode::GlobalAlloc(const std::string &arr, const BType &type, const std::deque<Operand> &init_vals)
{
    *pos << "\nglobal @" << arr << " = alloc " << type << ", ";
    if (init_vals.empty())
    {
        *pos << "zeroinit\n";
        return;
    }
    std::deque<int> arr_sizes = type.ArraySizes();
    int index = 0;
    PrintArray(arr_sizes, init_vals, index, 0);
    *pos << "\n";
}

void KoopaCode::Store(const Operand &reg_or_imm, const std::string &var, const bool &temp)
{
    *pos << "\tstore " << reg_or_imm << ", " << (temp ? "%" : "@") << var << "\n";
}

void KoopaCode::Store(const Operand &reg_or_imm, const Operand &elemptr)
{
    *pos << "\tstore " << reg_or_imm << ", " << elemptr << "\n";
}

void KoopaCode::Load(const Operand &reg, const std::string &var, const bool &temp)
{
    *pos << "\t" << reg << " = load " << (temp ? "%" : "@") << var << "\n";
}

void KoopaCode::Load(const Operand &reg, const Operand &elemptr)
{
    *pos << "\t" << reg << " = load " << elemptr << "\n";
}

void KoopaCode::GetElemptr(const Operand &elemptr, const std::string &arr, const Operand &index)
{
    *pos << "\t" << elemptr << " = getelemptr @" << arr << ", " << index << "\n";
}

void KoopaCode::GetElemptr(const Operand &elemptr, const Operand &base_ptr, const Operand &index)
{
    *pos << "\t" << elemptr << " = getelemptr " << base_ptr << ", " << index << "\n";
}

void KoopaCode::GetPtr(const Operand &elemptr, const Operand &ptr, const Operand &index)
{
    *pos << "\t" << elemptr << " = getptr " << ptr << ", " << index << "\n";
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