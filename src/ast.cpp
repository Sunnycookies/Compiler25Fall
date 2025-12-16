#include "ast.hpp"

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.Dump(os);
    return os;
}

AsOperand CompUnitAST::Dump(std::ostream &os) const
{
    func_def->Dump(os);
    return AsOperand();
}

AsOperand FuncDefAST::Dump(std::ostream &os) const
{
    os << "fun @" << ident << "(): ";
    func_type->Dump(os);
    os << " {\n";
    block->Dump(os);
    os << "}\n";
    return AsOperand();
}

AsOperand FuncTypeAST::Dump(std::ostream &os) const
{
    if (type == "int")
    {
        os << "i32";
    }
    return AsOperand();
}

AsOperand BlockAST::Dump(std::ostream &os) const
{
    os << "%entry:\n";
    for (size_t i = 0, len = block_items.size(); i < len; ++i)
    {
        block_items[i]->Dump(os);
    }
    return AsOperand();
}

AsOperand StmtAST::Dump(std::ostream &os) const
{
    AsOperand operant = exp->Dump(os);
    os << "\tret " << operant << "\n";
    return AsOperand();
}

AsOperand ExpAST::Dump(std::ostream &os) const
{
    return lor_exp->Dump(os);
}

AsOperand NumberAST::Dump(std::ostream &os) const
{
    return AsOperand(AsOperand::IMM, number);
}

AsOperand PrimaryExpAST::Dump(std::ostream &os) const
{
    return exp_or_lval_or_number->Dump(os);
}

AsOperand UnaryExpAST::Dump(std::ostream &os) const
{
    AsOperand operand = primary_or_unary_exp->Dump(os);
    if (type == PRIMARY_EXP)
    {
        return operand;
    }
    else if (operand.IsReg())
    {
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
    else
    {
        switch (unary_op[0])
        {
        case '+':
            return AsOperand(AsOperand::IMM, operand.ImmValue());
        case '-':
            return AsOperand(AsOperand::IMM, -operand.ImmValue());
        case '!':
            return AsOperand(AsOperand::IMM, !operand.ImmValue());
        default:
            assert(false);
        }
    }
    return AsOperand();
}

AsOperand MulExpAST::Dump(std::ostream &os) const
{
    if (type == UNARY_EXP)
    {
        return unary_exp->Dump(os);
    }
    AsOperand left = mul_exp->Dump(os);
    AsOperand right = unary_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
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
    else
    {
        switch (mul_op[0])
        {
        case '*':
            return AsOperand(AsOperand::IMM, left.ImmValue() * right.ImmValue());
        case '/':
            return AsOperand(AsOperand::IMM, left.ImmValue() / right.ImmValue());
        case '%':
            return AsOperand(AsOperand::IMM, left.ImmValue() % right.ImmValue());
        default:
            assert(false);
        }
    }
    return AsOperand();
}

AsOperand AddExpAST::Dump(std::ostream &os) const
{
    if (type == MUL_EXP)
    {
        return mul_exp->Dump(os);
    }
    AsOperand left = add_exp->Dump(os);
    AsOperand right = mul_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
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
    else
    {
        switch (add_op[0])
        {
        case '+':
            return AsOperand(AsOperand::IMM, left.ImmValue() + right.ImmValue());
        case '-':
            return AsOperand(AsOperand::IMM, left.ImmValue() - right.ImmValue());
            break;
        default:
            assert(false);
        }
    }
    return AsOperand();
}

AsOperand RelExpAST::Dump(std::ostream &os) const
{
    if (type == ADD_EXP)
    {
        return add_exp->Dump(os);
    }
    AsOperand left = rel_exp->Dump(os);
    AsOperand right = add_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        AsOperand result = AsOperand(AsOperand::REG);
        if (rel_op == "<")
        {
            os << "\t" << result << " = lt " << left << ", " << right << "\n";
        }
        else if (rel_op == ">")
        {
            os << "\t" << result << " = gt " << left << ", " << right << "\n";
        }
        else if (rel_op == "<=")
        {
            os << "\t" << result << " = le " << left << ", " << right << "\n";
        }
        else if (rel_op == ">=")
        {
            os << "\t" << result << " = ge " << left << ", " << right << "\n";
        }
        else
        {
            assert(false);
        }
        return result;
    }
    else
    {
        if (rel_op == "<")
        {
            return AsOperand(AsOperand::IMM, left.ImmValue() < right.ImmValue());
        }
        else if (rel_op == ">")
        {
            return AsOperand(AsOperand::IMM, left.ImmValue() > right.ImmValue());
        }
        else if (rel_op == "<=")
        {
            return AsOperand(AsOperand::IMM, left.ImmValue() <= right.ImmValue());
        }
        else if (rel_op == ">=")
        {
            return AsOperand(AsOperand::IMM, left.ImmValue() >= right.ImmValue());
        }
        else
        {
            assert(false);
        }
    }
    return AsOperand();
}

AsOperand EqExpAST::Dump(std::ostream &os) const
{
    if (type == REL_EXP)
    {
        return rel_exp->Dump(os);
    }
    AsOperand left = eq_exp->Dump(os);
    AsOperand right = rel_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        AsOperand result = AsOperand(AsOperand::REG);
        if (eq_op == "==")
        {
            os << "\t" << result << " = eq " << left << ", " << right << "\n";
        }
        else if (eq_op == "!=")
        {
            os << "\t" << result << " = ne " << left << ", " << right << "\n";
        }
        else
        {
            assert(false);
        }
        return result;
    }
    else
    {
        if (eq_op == "==")
        {
            return AsOperand(AsOperand::IMM, left.ImmValue() == right.ImmValue());
        }
        else if (eq_op == "!=")
        {
            return AsOperand(AsOperand::IMM, left.ImmValue() != right.ImmValue());
        }
        else
        {
            assert(false);
        }
    }
    return AsOperand();
}

AsOperand LAndExpAST::Dump(std::ostream &os) const
{
    if (type == EQ_EXP)
    {
        return eq_exp->Dump(os);
    }
    AsOperand left = land_exp->Dump(os);
    AsOperand right = eq_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        AsOperand bool_left = AsOperand(AsOperand::REG);
        AsOperand bool_right = AsOperand(AsOperand::REG);
        AsOperand result = AsOperand(AsOperand::REG);
        os << "\t" << bool_left << " = ne " << left << ", 0\n";
        os << "\t" << bool_right << " = ne " << right << ", 0\n";
        os << "\t" << result << " = and " << bool_left << ", " << bool_right << "\n";
        return result;
    }
    else
    {
        return AsOperand(AsOperand::IMM, left.ImmValue() && right.ImmValue());
    }
    return AsOperand();
}

AsOperand LOrExpAST::Dump(std::ostream &os) const
{
    if (type == LAND_EXP)
    {
        return land_exp->Dump(os);
    }
    AsOperand left = lor_exp->Dump(os);
    AsOperand right = land_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        AsOperand lr_or = AsOperand(AsOperand::REG);
        AsOperand result = AsOperand(AsOperand::REG);
        os << "\t" << lr_or << " = or " << left << ", " << right << "\n";
        os << "\t" << result << " = ne " << lr_or << ", 0\n";
        return result;
    }
    else
    {
        return AsOperand(AsOperand::IMM, left.ImmValue() || right.ImmValue());
    }
    return AsOperand();
}

AsOperand DeclAST::Dump(std::ostream &os) const
{
    const_decl->Dump(os);
    return AsOperand();
}

AsOperand ConstDeclAST::Dump(std::ostream &os) const
{
    for (size_t i = 0, len = const_defs.size(); i < len; ++i)
    {
        AsOperand const_value = const_defs[i]->Dump(os);
    }
    return AsOperand();
}

AsOperand BTypeAST::Dump(std::ostream &os) const
{
    os << "i32";
    return AsOperand();
}

AsOperand ConstDefAST::Dump(std::ostream &os) const
{
    AsOperand const_val = const_init_val->Dump(os);
    assert(!symbol_table->record(ident, Symbol(Symbol::LVAL, const_val.ImmValue())));
    return AsOperand();
}

AsOperand ConstInitValAST::Dump(std::ostream &os) const
{
    return const_exp->Dump(os);
}

AsOperand BlockItemAST::Dump(std::ostream &os) const
{
    return decl_or_stmt->Dump(os);
}

AsOperand LValAST::Dump(std::ostream &os) const
{
    assert(symbol_table->find(ident));
    Symbol symbol = symbol_table->get(ident);
    if (symbol.type == Symbol::LVAL)
    {
        return AsOperand(AsOperand::IMM, symbol.val);
    }
    return AsOperand();
}

AsOperand ConstExpAST::Dump(std::ostream &os) const
{
    return exp->Dump(os);
}