#include "ast.hpp"

int AsOperant::reg_count = 0;

AsOperant::AsOperant()
{
    type = IMM;
    value.imm_value = 0;
}

AsOperant::~AsOperant()
{
    // pass
}

AsOperant::AsOperant(const AsOperant &operant)
{
    type = operant.type;
    value = operant.value;
}

AsOperant::AsOperant(const operant_type &t, const int &v = 0)
{
    type = t;
    if (type == REG)
    {
        value.reg_no = reg_count++;
    }
    else if (type == IMM)
    {
        value.imm_value = v;
    }
}

std::ostream &operator<<(std::ostream &os, const AsOperant &operant)
{
    if (operant.type == AsOperant::REG)
    {
        os << "%" << operant.value.reg_no;
    }
    else if (operant.type == AsOperant::IMM)
    {
        os << operant.value.imm_value;
    }
    return os;
}

std::string BaseAST::mode = MODE_KOOPA;

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.Dump(os);
    return os;
}

AsOperant CompUnitAST::Dump(std::ostream &os = std::cout) const
{
    func_def->Dump(os);
    return AsOperant();
}

AsOperant FuncDefAST::Dump(std::ostream &os = std::cout) const
{
    os << "fun @" << ident << "(): ";
    func_type->Dump(os);
    os << " {\n";
    block->Dump(os);
    os << "}\n";
    return AsOperant();
}

AsOperant TypeAST::Dump(std::ostream &os = std::cout) const
{
    if (type == "int")
    {
        os << "i32";
    }
    return AsOperant();
}

AsOperant BlockAST::Dump(std::ostream &os = std::cout) const
{
    os << "%entry:\n";
    stmt->Dump(os);
    return AsOperant();
}

AsOperant StmtAST::Dump(std::ostream &os = std::cout) const
{
    AsOperant operant = exp->Dump(os);
    os << "\tret " << operant << "\n";
    return AsOperant();
}

AsOperant ExpAST::Dump(std::ostream &os = std::cout) const
{
    return unary_exp->Dump(os);
}

AsOperant NumberAST::Dump(std::ostream &os = std::cout) const
{
    return AsOperant(AsOperant::IMM, number);
}

AsOperant PrimaryExpAST::Dump(std::ostream &os = std::cout) const
{
    return exp_or_number->Dump(os);
}

AsOperant UnaryExpAST::Dump(std::ostream &os = std::cout) const
{
    if (type == PRIMARY_EXP)
    {
        return primary_or_unary_exp->Dump(os);
    }
    else if (type == UNARY_EXP)
    {
        AsOperant operand = primary_or_unary_exp->Dump(os);
        AsOperant result = AsOperant(AsOperant::REG);
        switch (unary_op[0])
        {
        case '+':
            os << "\t" << result << " = add 0, " << operand << "\n";
            break;

        case '-':
            os << "\t" << result << " = sub 0, " << operand << "\n";
            break;

        case '!':
            os << "\t" << result << " = eq " << operand << ", 0\n";
            break;

        default:
            assert(false);
        }
        return result;
    }
    return AsOperant();
}