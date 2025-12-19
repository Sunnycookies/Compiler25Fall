#include "ast.hpp"

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.Dump(os);
    return os;
}

/*
=================================================
- Program
=================================================
*/

Operand CompUnitAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "CompUnitAST Dump\n";
#endif

    func_def->Dump(os);
    return Operand();
}

/*
=================================================
- Variable Declaration & Definition
=================================================
*/

Operand DeclAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Decl Dump\n";
#endif

    const_or_var_decl->Dump(os);
    return Operand();
}

Operand ConstDeclAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstDecl Dump\n";
#endif

    for (size_t i = 0, len = const_defs.size(); i < len; ++i)
    {
        Operand const_value = const_defs[i]->Dump(os);
    }
    return Operand();
}

Operand BTypeAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "BType Dump\n";
#endif

    os << "i32";
    return Operand();
}

Operand ConstDefAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstDef Dump\n";
#endif

    Operand const_val = const_init_val->Dump(os);
    symbol_tables->InnerBlockRecord(ident, Symbol(Symbol::CONST, const_val.ImmValue()));
    return Operand();
}

Operand ConstInitValAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstInitVal Dump\n";
#endif

    return const_exp->Dump(os);
}

Operand VarDeclAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "VarDecl Dump\n";
#endif

    for (size_t i = 0, len = var_defs.size(); i < len; ++i)
    {
        ((VarDefAST *)(var_defs[i].get()))->DumpWithType(os, *b_type);
    }
    return Operand();
}

Operand VarDefAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "VarDef Dump\n";
#endif

    if (type == INITVAL)
    {
        Operand val = init_val->Dump(os);
        Symbol symbol = symbol_tables->Get(ident);
        os << "\tstore " << val << ", @" << ident << "_" << symbol.val << '\n';
    }
    return Operand();
}

Operand VarDefAST::DumpWithType(std::ostream &os, const BaseAST &type) const
{
#ifdef DEBUG
    debug << "VarDef DumpWithType\n";
#endif

    symbol_tables->InnerBlockRecord(ident, Symbol(Symbol::VAR));
    if (!symbol_tables->InterBlockAllocated(ident))
    {
        Symbol symbol = symbol_tables->Get(ident);
        os << "\t@" << ident << "_" << symbol.val << " = alloc " << type << "\n";
        symbol_tables->InterBlockAllocate(ident);
    }
    return Dump(os);
}

Operand InitValAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "InitVal Dump\n";
#endif

    return exp->Dump(os);
}

/*
=================================================
- Fuction Declaration
=================================================
*/

Operand FuncDefAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "FuncDef Dump\n";
#endif

    os << "fun @" << ident << "(): ";
    func_type->Dump(os);
    os << " {\n";
    os << "%entry:\n";
    Operand return_val = block->Dump(os);
    if (!return_val.IsReturnMark())
    {
        os << "\tret\n";
    }
    os << "}\n";
    return Operand();
}

Operand FuncTypeAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "FuncType Dump\n";
#endif

    if (type == "int")
    {
        os << "i32";
    }
    return Operand();
}

/*
=================================================
- Block
=================================================
*/

Operand BlockAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Block Dump\n";
#endif

    symbol_tables->NewSymbolTable();
    for (size_t i = 0, len = block_items.size(); i < len; ++i)
    {
        Operand return_val = block_items[i]->Dump(os);
        if (return_val.IsReturnMark())
        {
            symbol_tables->DeleteSymbolTable();
            return return_val;
        }
    }
    symbol_tables->DeleteSymbolTable();
    return Operand();
}

Operand BlockItemAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "BlockItem Dump\n";
#endif

    return decl_or_stmt->Dump(os);
}

Operand StmtAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Stmt Dump\n";
#endif

    if (type == RETURN)
    {
#ifdef DEBUG
        debug << "\tStmt - RETURN\n";
#endif
        if (exp)
        {
            Operand operand = exp->Dump(os);
            os << "\tret " << operand << "\n";
        }
        else
        {
            os << "\tret\n";
        }
        return Operand().SetAsReturnMark();
    }

    else if (type == LVAL)
    {
#ifdef DEBUG
        debug << "\tStmt - LVAL\n";
#endif
        Operand operand = exp->Dump(os);
        std::string val_name = ((LValAST *)(lval_or_block.get()))->ident;
        Symbol symbol = symbol_tables->Get(val_name);
        assert(symbol.type == Symbol::VAR && symbol.val);
        os << "\tstore " << operand << ", @" << val_name << "_" << symbol.val << "\n";
    }

    else if (type == EXP && exp)
    {
#ifdef DEBUG
        debug << "\tStmt - EXP\n";
#endif
        return exp->Dump(os);
    }

    else if (type == BLOCK)
    {
#ifdef DEBUG
        debug << "\tStmt - BLOCK\n";
#endif
        return lval_or_block->Dump(os);
    }

    else if (type == IF)
    {
#ifdef DEBUG
        debug << "\tStmt - IF\n";
#endif
        Operand cond = exp->Dump(os);
        if (!cond.IsReg())
        {
            if (cond.ImmValue())
            {
                return stmt->Dump(os);
            }
            else if (else_stmt)
            {
                return else_stmt->Dump(os);
            }
            return Operand();
        }

        int branch_mark = symbol_tables->NewBranchMark();
        std::string label_then = "%then_" + std::to_string(branch_mark);
        std::string label_else = "%else_" + std::to_string(branch_mark);
        std::string label_end = "%end_" + std::to_string(branch_mark);

        os << "\tbr " << cond << ", " << label_then << ", " << (else_stmt ? label_else : label_end) << "\n";

        os << label_then << ":\n";
        Operand then_returned = stmt->Dump(os);
        if (!then_returned.IsReturnMark())
        {
            os << "\tjump " << label_end << "\n";
        }

        Operand else_returned = Operand();
        if (else_stmt)
        {
            os << label_else << ":\n";
            else_returned = else_stmt->Dump(os);
            if (!else_returned.IsReturnMark())
            {
                os << "\tjump " << label_end << "\n";
            }
        }

        if (then_returned.IsReturnMark() && else_returned.IsReturnMark())
        {
            return Operand().SetAsReturnMark();
        }
        else
        {
            os << label_end << ":\n";
        }
    }
    return Operand();
}

/*
=================================================
- Expression
=================================================
*/

Operand ExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Exp Dump\n";
#endif

    return lor_exp->Dump(os);
}

Operand LValAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "LVal Dump\n";
#endif

    Symbol symbol = symbol_tables->Get(ident);
    assert(symbol.type == Symbol::CONST || symbol.val);
    if (symbol.type == Symbol::CONST)
    {
        return Operand(Operand::IMM, symbol.val);
    }
    else if (symbol.type == Symbol::VAR)
    {
        Operand var_reg = Operand(Operand::REG);
        os << "\t" << var_reg << " = load @" << ident << "_" << symbol.val << "\n";
        return var_reg;
    }
    return Operand();
}

Operand PrimaryExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "PrimaryExp Dump\n";
#endif

    return exp_or_lval_or_number->Dump(os);
}

Operand NumberAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "Number Dump\n";
#endif

    return Operand(Operand::IMM, number);
}

Operand UnaryExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "UnaryExp Dump\n";
#endif

    Operand operand = primary_or_unary_exp->Dump(os);
    if (type == PRIMARY_EXP)
    {
        return operand;
    }
    else if (operand.IsReg())
    {
        Operand result = Operand(Operand::REG);
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
            return Operand(Operand::IMM, operand.ImmValue());
        case '-':
            return Operand(Operand::IMM, -operand.ImmValue());
        case '!':
            return Operand(Operand::IMM, !operand.ImmValue());
        default:
            assert(false);
        }
    }
    return Operand();
}

Operand MulExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "MulExp Dump\n";
#endif

    if (type == UNARY_EXP)
    {
        return unary_exp->Dump(os);
    }
    Operand left = mul_exp->Dump(os);
    Operand right = unary_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        Operand result = Operand(Operand::REG);
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
            return Operand(Operand::IMM, left.ImmValue() * right.ImmValue());
        case '/':
            return Operand(Operand::IMM, left.ImmValue() / right.ImmValue());
        case '%':
            return Operand(Operand::IMM, left.ImmValue() % right.ImmValue());
        default:
            assert(false);
        }
    }
    return Operand();
}

Operand AddExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "AddExp Dump\n";
#endif

    if (type == MUL_EXP)
    {
        return mul_exp->Dump(os);
    }
    Operand left = add_exp->Dump(os);
    Operand right = mul_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        Operand result = Operand(Operand::REG);
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
            return Operand(Operand::IMM, left.ImmValue() + right.ImmValue());
        case '-':
            return Operand(Operand::IMM, left.ImmValue() - right.ImmValue());
            break;
        default:
            assert(false);
        }
    }
    return Operand();
}

Operand RelExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "RelExp Dump\n";
#endif

    if (type == ADD_EXP)
    {
        return add_exp->Dump(os);
    }
    Operand left = rel_exp->Dump(os);
    Operand right = add_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        Operand result = Operand(Operand::REG);
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
            return Operand(Operand::IMM, left.ImmValue() < right.ImmValue());
        }
        else if (rel_op == ">")
        {
            return Operand(Operand::IMM, left.ImmValue() > right.ImmValue());
        }
        else if (rel_op == "<=")
        {
            return Operand(Operand::IMM, left.ImmValue() <= right.ImmValue());
        }
        else if (rel_op == ">=")
        {
            return Operand(Operand::IMM, left.ImmValue() >= right.ImmValue());
        }
        else
        {
            assert(false);
        }
    }
    return Operand();
}

Operand EqExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "EqExp Dump\n";
#endif

    if (type == REL_EXP)
    {
        return rel_exp->Dump(os);
    }
    Operand left = eq_exp->Dump(os);
    Operand right = rel_exp->Dump(os);
    if (left.IsReg() || right.IsReg())
    {
        Operand result = Operand(Operand::REG);
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
            return Operand(Operand::IMM, left.ImmValue() == right.ImmValue());
        }
        else if (eq_op == "!=")
        {
            return Operand(Operand::IMM, left.ImmValue() != right.ImmValue());
        }
        else
        {
            assert(false);
        }
    }
    return Operand();
}

Operand LAndExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "LAndExp Dump\n";
#endif

    if (type == EQ_EXP)
    {
        return eq_exp->Dump(os);
    }

    Operand left = land_exp->Dump(os);
    if (!left.IsReg())
    {
        if (left.ImmValue() == 0)
        {
            return Operand(Operand::IMM, 0);
        }

        Operand right = eq_exp->Dump(os);

        if (right.IsReg())
        {
            Operand result = Operand(Operand::REG);
            os << "\t" << result << " = ne " << right << ", 0\n";
            return result;
        }

        return Operand(Operand::IMM, right.ImmValue() != 0);
    }

    Operand cond = Operand(Operand::REG);
    int branch_mark = symbol_tables->NewBranchMark();
    std::string and_then = "%and_then_" + std::to_string(branch_mark);
    std::string and_else = "%and_else_" + std::to_string(branch_mark);
    std::string and_end = "%and_end_" + std::to_string(branch_mark);
    std::string and_result = "%and_result_" + std::to_string(branch_mark);

    symbol_tables->InnerBlockRecord(and_result, Symbol(Symbol::VAR));
    if (!symbol_tables->InterBlockAllocated(and_result))
    {
        os << "\t" << and_result << " = alloc i32\n";
        symbol_tables->InterBlockAllocate(and_result);
    }
    os << "\t" << cond << " = ne " << left << ", 0\n";
    os << "\tbr " << cond << ", " << and_then << ", " << and_else << "\n";

    os << and_then << ":\n";
    Operand right = eq_exp->Dump(os);
    if (right.IsReg())
    {
        Operand temp = Operand(Operand::REG);
        os << "\t" << temp << " = ne " << right << ", 0\n";
        os << "\tstore " << temp << ", " << and_result << "\n";
    }
    else
    {
        os << "\tstore " << (right.ImmValue() != 0) << ", " << and_result << "\n";
    }
    os << "\tjump " << and_end << "\n";

    os << and_else << ":\n";
    os << "\tstore 0, " << and_result << "\n";
    os << "\tjump " << and_end << "\n";

    os << and_end << ":\n";
    Operand result = Operand(Operand::REG);
    os << "\t" << result << " = load " << and_result << "\n";

    return result;
}

Operand LOrExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "LOrExp Dump\n";
#endif

    if (type == LAND_EXP)
    {
        return land_exp->Dump(os);
    }

    Operand left = lor_exp->Dump(os);
    if (!left.IsReg())
    {
        if (left.ImmValue() != 0)
        {
            return Operand(Operand::IMM, 1);
        }

        Operand right = land_exp->Dump(os);
        if (right.IsReg())
        {
            Operand result = Operand(Operand::REG);
            os << "\t" << result << " = ne " << right << ", 0\n";
            return result;
        }

        return Operand(Operand::IMM, right.ImmValue() != 0);
    }

    Operand cond = Operand(Operand::REG);
    int branch_mark = symbol_tables->NewBranchMark();
    std::string or_then = "%or_then_" + std::to_string(branch_mark);
    std::string or_else = "%or_else_" + std::to_string(branch_mark);
    std::string or_end = "%or_end_" + std::to_string(branch_mark);
    std::string or_result = "%or_result_" + std::to_string(branch_mark);

    symbol_tables->InnerBlockRecord(or_result, Symbol(Symbol::VAR));
    if (!symbol_tables->InterBlockAllocated(or_result))
    {
        os << "\t" << or_result << " = alloc i32\n";
        symbol_tables->InterBlockAllocate(or_result);
    }
    os << "\t" << cond << " = ne " << left << ", 0\n";
    os << "\tbr " << cond << ", " << or_then << ", " << or_else << "\n";

    os << or_then << ":\n";
    os << "\tstore 1, " << or_result << "\n";
    os << "\tjump " << or_end << "\n";

    os << or_else << ":\n";
    Operand right = land_exp->Dump(os);
    if (right.IsReg())
    {
        Operand temp = Operand(Operand::REG);
        os << "\t" << temp << " = ne " << right << ", 0\n";
        os << "\tstore " << temp << ", " << or_result << "\n";
    }
    else
    {
        os << "\tstore " << (right.ImmValue() != 0) << ", " << or_result << "\n";
    }
    os << "\tjump " << or_end << "\n";

    os << or_end << ":\n";
    Operand result = Operand(Operand::REG);
    os << "\t" << result << " = load " << or_result << "\n";

    return result;
}

Operand ConstExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstExp Dump\n";
#endif

    return exp->Dump(os);
}