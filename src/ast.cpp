#include "ast.hpp"

KoopaCode *BaseAST::printer = new KoopaCode();

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.printer->SetOstream(os);
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

    printer->Int();
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
        printer->Store(val, symbol_tables->Mark(ident, symbol.val), false);
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
        printer->Alloc(symbol_tables->Mark(ident, symbol.val), false);
        os << type;
        printer->NewLine();
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
    printer->FrontCurBrac();
    printer->Label("entry");
    Operand return_val = block->Dump(os);
    if (!return_val.IsReturnMark())
    {
        printer->Ret();
    }
    printer->EndCurBrac();
    return Operand();
}

Operand FuncTypeAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "FuncType Dump\n";
#endif

    if (type == "int")
    {
        printer->Int();
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
        if (return_val.IsLoopInterruption())
        {
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

std::string StmtAST::branch_then = "then";

std::string StmtAST::branch_else = "else";

std::string StmtAST::branch_end = "end";

std::string StmtAST::loop_entry = "loop_entry";

std::string StmtAST::loop_body = "loop_body";

std::string StmtAST::loop_end = "loop_end";

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
            printer->RetV(exp->Dump(os));
        }
        else
        {
            printer->Ret();
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
        printer->Store(operand, symbol_tables->Mark(val_name, symbol.val), false);
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
        std::string label_then = symbol_tables->Mark(branch_then, branch_mark);
        std::string label_else = symbol_tables->Mark(branch_else, branch_mark);
        std::string label_end = symbol_tables->Mark(branch_end, branch_mark);

        printer->Br(cond, label_then, (else_stmt ? label_else : label_end));

        printer->Label(label_then);
        Operand then_returned = stmt->Dump(os);
        if (then_returned.IsNormal())
        {
            printer->Jump(label_end);
        }

        Operand else_returned = Operand();
        if (else_stmt)
        {
            printer->Label(label_else);
            else_returned = else_stmt->Dump(os);
            if (else_returned.IsNormal())
            {
                printer->Jump(label_end);
            }
        }

        if (then_returned.IsNormal() || else_returned.IsNormal())
        {
            printer->Label(label_end);
        }
        else if (then_returned.IsReturnMark() && else_returned.IsReturnMark())
        {
            return Operand().SetAsReturnMark();
        }
        else
        {
            return Operand().SetAsLoopInterruption();
        }
    }

    else if (type == WHILE)
    {
#ifdef DEBUG
        debug << "\tStmt - WHILE\n";
#endif
        int branch_mark = symbol_tables->NewBranchMark();
        std::string while_entry = symbol_tables->Mark(loop_entry, branch_mark);
        std::string while_body = symbol_tables->Mark(loop_body, branch_mark);
        std::string while_end = symbol_tables->Mark(loop_end, branch_mark);

        symbol_tables->PushLoop(branch_mark);

        printer->Jump(while_entry);
        printer->Label(while_entry);
        Operand cond = exp->Dump(os);
        if (cond.IsReg())
        {
            printer->Br(cond, while_body, while_end);
        }
        else if (cond.ImmValue())
        {
            printer->Jump(while_body);
        }
        else
        {
            printer->Jump(while_end);
            printer->Label(while_end);
            return Operand();
        }

        printer->Label(while_body);
        Operand return_val = stmt->Dump(os);
        if (return_val.IsNormal())
        {
            printer->Jump(while_entry);
        }
        else if (!cond.IsReg() && return_val.IsReturnMark())
        {
            return Operand().SetAsReturnMark();
        }

        printer->Label(while_end);
        symbol_tables->PopLoop();
    }

    else if (type == BREAK)
    {
#ifdef DEBUG
        debug << "\tStmt - Break\n";
#endif
        printer->Jump(symbol_tables->Mark(loop_end, symbol_tables->GetTopLoop()));
        return Operand().SetAsLoopInterruption();
    }

    else if (type == CONTINUE)
    {
#ifdef DEBUG
        debug << "\tStmt - Continue\n";
#endif
        printer->Jump(symbol_tables->Mark(loop_entry, symbol_tables->GetTopLoop()));
        return Operand().SetAsLoopInterruption();
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
        printer->Load(var_reg, symbol_tables->Mark(ident, symbol.val), false);
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
            printer->Add(result, Operand(), operand);
            break;
        case '-':
            printer->Sub(result, Operand(), operand);
            break;
        case '!':
            printer->Eq(result, operand, Operand());
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
            printer->Mul(result, left, right);
            break;
        case '/':
            printer->Div(result, left, right);
            break;
        case '%':
            printer->Mod(result, left, right);
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
            printer->Add(result, left, right);
            break;
        case '-':
            printer->Sub(result, left, right);
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
            printer->Lt(result, left, right);
        }
        else if (rel_op == ">")
        {
            printer->Gt(result, left, right);
        }
        else if (rel_op == "<=")
        {
            printer->Le(result, left, right);
        }
        else if (rel_op == ">=")
        {
            printer->Ge(result, left, right);
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
            printer->Eq(result, left, right);
        }
        else if (eq_op == "!=")
        {
            printer->Ne(result, left, right);
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

std::string LAndExpAST::and_then = "and_then";

std::string LAndExpAST::and_else = "and_else";

std::string LAndExpAST::and_end = "and_end";

std::string LAndExpAST::and_temp = "and_temp";

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
            printer->Ne(result, right, Operand());
            return result;
        }

        return right != 0;
    }

    Operand cond = Operand(Operand::REG);
    int branch_mark = symbol_tables->NewBranchMark();
    std::string label_then = symbol_tables->Mark(and_then, branch_mark);
    std::string label_else = symbol_tables->Mark(and_else, branch_mark);
    std::string label_end = symbol_tables->Mark(and_end, branch_mark);
    std::string temp_result = symbol_tables->Mark(and_temp, branch_mark);

    symbol_tables->InnerBlockRecord(temp_result, Symbol(Symbol::VAR));
    if (!symbol_tables->InterBlockAllocated(temp_result))
    {
        printer->Alloc(temp_result);
        printer->Int();
        printer->NewLine();
        symbol_tables->InterBlockAllocate(temp_result);
    }
    printer->Ne(cond, left, Operand());
    printer->Br(cond, label_then, label_else);

    printer->Label(label_then);
    Operand right = eq_exp->Dump(os);
    if (right.IsReg())
    {
        Operand temp = Operand(Operand::REG);
        printer->Ne(temp, right, Operand());
        printer->Store(temp, temp_result);
    }
    else
    {
        printer->Store(right != 0, temp_result);
    }
    printer->Jump(label_end);

    printer->Label(label_else);
    printer->Store(Operand(), temp_result);
    printer->Jump(label_end);

    printer->Label(label_end);
    Operand result = Operand(Operand::REG);
    printer->Load(result, temp_result);

    return result;
}

std::string LOrExpAST::or_then = "or_then";

std::string LOrExpAST::or_else = "or_else";

std::string LOrExpAST::or_end = "or_end";

std::string LOrExpAST::or_temp = "or_temp";

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
        if (left.ImmValue())
        {
            return Operand(Operand::IMM, 1);
        }

        Operand right = land_exp->Dump(os);
        if (right.IsReg())
        {
            Operand result = Operand(Operand::REG);
            printer->Ne(result, right, Operand());
            return result;
        }

        return right != 0;
    }

    Operand cond = Operand(Operand::REG);
    int branch_mark = symbol_tables->NewBranchMark();
    std::string label_then = symbol_tables->Mark(or_then, branch_mark);
    std::string label_else = symbol_tables->Mark(or_else, branch_mark);
    std::string label_end = symbol_tables->Mark(or_end, branch_mark);
    std::string temp_result = symbol_tables->Mark(or_temp, branch_mark);

    symbol_tables->InnerBlockRecord(temp_result, Symbol(Symbol::VAR));
    if (!symbol_tables->InterBlockAllocated(temp_result))
    {
        printer->Alloc(temp_result);
        printer->Int();
        printer->NewLine();
        symbol_tables->InterBlockAllocate(temp_result);
    }
    printer->Ne(cond, left, Operand());
    printer->Br(cond, label_then, label_else);

    printer->Label(label_then);
    printer->Store(Operand(Operand::IMM, 1), temp_result);
    printer->Jump(label_end);

    printer->Label(label_else);
    Operand right = land_exp->Dump(os);
    if (right.IsReg())
    {
        Operand temp = Operand(Operand::REG);
        printer->Ne(temp, right, Operand());
        printer->Store(temp, temp_result);
    }
    else
    {
        printer->Store(right != 0, temp_result);
    }
    printer->Jump(label_end);

    printer->Label(label_end);
    Operand result = Operand(Operand::REG);
    printer->Load(result, temp_result);

    return result;
}

Operand ConstExpAST::Dump(std::ostream &os) const
{
#ifdef DEBUG
    debug << "ConstExp Dump\n";
#endif

    return exp->Dump(os);
}