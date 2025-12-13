#include "ast.hpp"

int AsOperand::reg_count = 0;

AsOperand::AsOperand()
{
    type = IMM;
    value.imm_value = 0;
}

AsOperand::~AsOperand()
{
    // pass
}

AsOperand::AsOperand(const AsOperand &operant)
{
    type = operant.type;
    value = operant.value;
}

AsOperand::AsOperand(const operant_type &t, const int &v = 0)
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

std::ostream &operator<<(std::ostream &os, const AsOperand &operant)
{
    if (operant.type == AsOperand::REG)
    {
        os << "%" << operant.value.reg_no;
    }
    else if (operant.type == AsOperand::IMM)
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

AsOperand CompUnitAST::Dump(std::ostream &os = std::cout) const
{
    func_def->Dump(os);
    return AsOperand();
}

AsOperand FuncDefAST::Dump(std::ostream &os = std::cout) const
{
    os << "fun @" << ident << "(): ";
    func_type->Dump(os);
    os << " {\n";
    block->Dump(os);
    os << "}\n";
    return AsOperand();
}

AsOperand TypeAST::Dump(std::ostream &os = std::cout) const
{
    if (type == "int")
    {
        os << "i32";
    }
    return AsOperand();
}

AsOperand BlockAST::Dump(std::ostream &os = std::cout) const
{
    os << "%entry:\n";
    stmt->Dump(os);
    return AsOperand();
}

AsOperand StmtAST::Dump(std::ostream &os = std::cout) const
{
    AsOperand operant = exp->Dump(os);
    os << "\tret " << operant << "\n";
    return AsOperand();
}

AsOperand ExpAST::Dump(std::ostream &os = std::cout) const
{
    return add_exp->Dump(os);
}

AsOperand NumberAST::Dump(std::ostream &os = std::cout) const
{
    return AsOperand(AsOperand::IMM, number);
}

AsOperand PrimaryExpAST::Dump(std::ostream &os = std::cout) const
{
    return exp_or_number->Dump(os);
}

AsOperand UnaryExpAST::Dump(std::ostream &os = std::cout) const
{
    if (type == PRIMARY_EXP)
    {
        return primary_or_unary_exp->Dump(os);
    }
    else if (type == UNARY_EXP)
    {
        AsOperand operand = primary_or_unary_exp->Dump(os);
        AsOperand result = AsOperand(AsOperand::REG);
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
    return AsOperand();
}

AsOperand MulExpAST::Dump(std::ostream &os = std::cout) const
{
    if (type == UNARY_EXP)
    {
        return unary_exp->Dump(os);
    }
    else if (type == BINARY_EXP)
    {
        AsOperand left = mul_exp->Dump(os);
        AsOperand right = unary_exp->Dump(os);
        AsOperand result = AsOperand(AsOperand::REG);
        switch (mul_op[0])
        {
        case '*':
            os << "\t" << result << " = mul " << left << ", " << right << "\n";
            break;

        case '/':
            os << "\t" << result << " = div " << left << ", " << right << "\n";
            break;

        case '%':
            os << "\t" << result << " = mod " << left << ", " << right << "\n";
            break;

        default:
            assert(false);
        }
        return result;
    }
    return AsOperand();
}

AsOperand AddExpAST::Dump(std::ostream &os = std::cout) const
{
    if (type == MUL_EXP)
    {
        return mul_exp->Dump(os);
    }
    else if (type == BINARY_EXP)
    {
        AsOperand left = add_exp->Dump(os);
        AsOperand right = mul_exp->Dump(os);
        AsOperand result = AsOperand(AsOperand::REG);
        switch (add_op[0])
        {
        case '+':
            os << "\t" << result << " = add " << left << ", " << right << "\n";
            break;

        case '-':
            os << "\t" << result << " = sub " << left << ", " << right << "\n";
            break;

        default:
            assert(false);
        }
        return result;
    }
    return AsOperand();
}