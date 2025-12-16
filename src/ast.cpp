#include "ast.hpp"

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.Dump(os);
    return os;
}

AsOperand CompUnitAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "CompUnitAST Dump\n";
#endif

    func_def->Dump(os);
    return AsOperand();
}

AsOperand FuncDefAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "FuncDef Dump\n";
#endif

    os << "fun @" << ident << "(): ";
    func_type->Dump(os);
    os << " {\n";
    block->Dump(os);
    os << "}\n";
    return AsOperand();
}

AsOperand FuncTypeAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "FuncType Dump\n";
#endif

    if (type == "int")
    {
        os << "i32";
    }
    return AsOperand();
}

AsOperand BlockAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Block Dump\n";
#endif

    os << "%entry:\n";
    for (size_t i = 0, len = block_items.size(); i < len; ++i)
    {
        block_items[i]->Dump(os);
    }
    return AsOperand();
}

AsOperand StmtAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Stmt Dump\n";
#endif

    AsOperand operant = exp->Dump(os);
    if (type == RETURN)
    {
        os << "\tret " << operant << "\n";
    }
    else if (type == LVAL)
    {
        std::string val_name = ((LValAST*)(lval.get()))->ident;
        assert(symbol_table->find(val_name));
        os << "\tstore " << operant << ", @" << val_name << "\n";
    }
    return AsOperand();
}

AsOperand ExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Exp Dump\n";
#endif

    return lor_exp->Dump(os);
}

AsOperand NumberAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Number Dump\n";
#endif

    return AsOperand(AsOperand::IMM, number);
}

AsOperand PrimaryExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "PrimaryExp Dump\n";
#endif

    return exp_or_lval_or_number->Dump(os);
}

AsOperand UnaryExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "UnaryExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "MulExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "AddExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "RelExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "EqExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "LAndExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "LOrExp Dump\n";
#endif

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
#ifdef DEBUG
    debug << "Decl Dump\n";
#endif

    const_or_var_decl->Dump(os);
    return AsOperand();
}

AsOperand ConstDeclAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstDecl Dump\n";
#endif

    for (size_t i = 0, len = const_defs.size(); i < len; ++i)
    {
        AsOperand const_value = const_defs[i]->Dump(os);
    }
    return AsOperand();
}

AsOperand BTypeAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "BType Dump\n";
#endif

    os << "i32";
    return AsOperand();
}

AsOperand ConstDefAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstDef Dump\n";
#endif

    AsOperand const_val = const_init_val->Dump(os);
    assert(!symbol_table->record(ident, Symbol(Symbol::CONST, const_val.ImmValue())));
    return AsOperand();
}

AsOperand ConstInitValAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstInitVal Dump\n";
#endif

    return const_exp->Dump(os);
}

AsOperand BlockItemAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "BlockItem Dump\n";
#endif

    return decl_or_stmt->Dump(os);
}

AsOperand LValAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "LVal Dump\n";
#endif

    assert(symbol_table->find(ident));
    Symbol symbol = symbol_table->get(ident);
    if (symbol.type == Symbol::CONST)
    {
        return AsOperand(AsOperand::IMM, symbol.val);
    } else if (symbol.type == Symbol::VAR)
    {
        AsOperand var_reg = AsOperand(AsOperand::REG);
        os << "\t" << var_reg << " = load @" << ident << "\n";
        return var_reg;
    }
    return AsOperand();
}

AsOperand ConstExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstExp Dump\n";
#endif

    return exp->Dump(os);
}

AsOperand VarDeclAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "VarDecl Dump\n";
#endif

    for(size_t i = 0, len = var_defs.size(); i < len; ++i)
    {
        ((VarDefAST*)(var_defs[i].get()))->DumpWithType(os, *b_type);
    }
    return AsOperand();
}

AsOperand VarDefAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "VarDef Dump\n";
#endif

    if (type == INITVAL)
    {
        AsOperand val = init_val->Dump(os);
        os << "\tstore " << val << ", @" << ident << '\n';
    }
    assert(!symbol_table->record(ident, Symbol(Symbol::VAR)));
    return AsOperand();
}

AsOperand VarDefAST::DumpWithType(std::ostream &os, const BaseAST &type) const
{
#ifdef DEBUG
    debug << "VarDef DumpWithType\n";
#endif

    os << "\t@" << ident << " = alloc " << type << "\n";
    return Dump(os);
}

AsOperand InitValAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "InitVal Dump\n";
#endif

    return exp->Dump(os);
}