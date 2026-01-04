#include "ast.hpp"

KoopaCode *BaseAST::printer = new KoopaCode();

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.printer->SetOstream(os);
    ast.Dump();
    return os;
}

/*
=================================================
- Program
=================================================
*/

Operand ProgramAST::Dump() const
{
#ifdef DEBUG
    debug << "Program Dump\n";
#endif

    symbol_tables->NewSymbolTable();
    printer->DeclFunc("getint", {BType::INT});
    printer->DeclFunc("getch", {BType::INT});
    printer->DeclFunc("getarray", {BType::INT, BType(BType::INT, {Operand()})});
    printer->DeclFunc("putint", {BType::VOID, BType::INT});
    printer->DeclFunc("putch", {BType::VOID, BType::INT});
    printer->DeclFunc("putarray", {BType::VOID, BType::INT, BType(BType::INT, {Operand()})});
    printer->DeclFunc("starttime", {BType::VOID});
    printer->DeclFunc("stoptime", {BType::VOID});
    symbol_tables->RecordSymbol("getint", Symbol(Symbol::FUNC, BType(BType::INT)));
    symbol_tables->RecordSymbol("getch", Symbol(Symbol::FUNC, BType(BType::INT)));
    symbol_tables->RecordSymbol("getarray", Symbol(Symbol::FUNC, BType(BType::INT)));
    symbol_tables->RecordSymbol("putint", Symbol(Symbol::FUNC, BType(BType::VOID)));
    symbol_tables->RecordSymbol("putch", Symbol(Symbol::FUNC, BType(BType::VOID)));
    symbol_tables->RecordSymbol("putarray", Symbol(Symbol::FUNC, BType(BType::VOID)));
    symbol_tables->RecordSymbol("starttime", Symbol(Symbol::FUNC, BType(BType::VOID)));
    symbol_tables->RecordSymbol("stoptime", Symbol(Symbol::FUNC, BType(BType::VOID)));
    for (int i = 0, n = comp_units.size(); i < n; ++i)
    {
        comp_units[i]->Dump();
    }
    symbol_tables->DeleteSymbolTable();
    return Operand();
}

Operand CompUnitAST::Dump() const
{
#ifdef DEBUG
    debug << "CompUnitAST Dump\n";
#endif

    func_def_or_decl->Dump();
    return Operand();
}

/*
=================================================
- Variable Declaration & Definition
=================================================
*/

bool DeclAST::global = true;

Operand DeclAST::Dump() const
{
#ifdef DEBUG
    debug << "Decl Dump\n";
#endif

    const_or_var_decl->Dump();
    return Operand();
}

Operand ConstDeclAST::Dump() const
{
#ifdef DEBUG
    debug << "ConstDecl Dump\n";
#endif

    ConstDefAST::base_type = type;
    for (int i = 0, n = const_defs.size(); i < n; ++i)
    {
        Operand const_value = const_defs[i]->Dump();
    }
    return Operand();
}

BType ConstDefAST::base_type = BType();

Operand ConstDefAST::Dump() const
{
#ifdef DEBUG
    debug << "ConstDef Dump\n";
#endif

    assert(const_init_val.get());

    if (array_sizes.empty())
    {
        Operand const_val = const_init_val->Dump();
        symbol_tables->RecordSymbol(ident, Symbol(Symbol::CONST, const_val.ImmValue()));
        return Operand();
    }

    std::deque<Operand> sizes;
    for (int i = 0, n = array_sizes.size(); i < n; ++i)
    {
        sizes.push_back(array_sizes[i]->Dump());
    }
    BType arr_type = BType(base_type.type, sizes);
    symbol_tables->RecordSymbol(ident, Symbol(Symbol::CONST_ARRAY, arr_type));

    if (DeclAST::global && ((ConstInitValAST *)(const_init_val.get()))->const_init_vals.empty())
    {
        printer->GlobalAlloc(symbol_tables->GetName(ident), arr_type, std::deque<Operand>());
        return Operand();
    }

    std::deque<int> steps({1});
    for (int i = sizes.size() - 1, step = 1; i >= 0; --i)
    {
        step *= sizes[i].ImmValue();
        steps.push_front(step);
    }

    std::deque<Operand> vals;
    int index = 0;
    ((ConstInitValAST *)(const_init_val.get()))->Initialize(steps, vals, index, 0);
    if (DeclAST::global)
    {
        printer->GlobalAlloc(symbol_tables->GetName(ident), arr_type, vals);
    }
    else
    {
        printer->Alloc(symbol_tables->GetName(ident), arr_type, vals);
    }
    return Operand();
}

Operand ConstInitValAST::Dump() const
{
#ifdef DEBUG
    debug << "ConstInitVal Dump\n";
#endif

    assert(type == CONST_EXP);
    return const_exp->Dump();
}

void ConstInitValAST::Initialize(const std::deque<int> &steps, std::deque<Operand> &vals, int &index, const int &dim) const
{
#ifdef DEBUG
    debug << "ConstInitVal Initialize\n";
#endif

    assert(type == INITIALIZER || const_exp.get());

    if (type == CONST_EXP)
    {
        Operand value = const_exp->Dump();
        assert(!value.IsReg());
        vals.push_back(value);
        index++;
        return;
    }

    int steps_size = steps.size();
    assert(steps[steps_size - 1] == 1);

    int target = index + steps[dim];
    for (int i = 0, n = const_init_vals.size(); i < n; ++i)
    {
        int j = dim + 1;
        assert(j < steps_size);
        while (index % steps[j])
        {
            ++j;
        }
        ((ConstInitValAST *)(const_init_vals[i].get()))->Initialize(steps, vals, index, j);
    }
#ifdef DEBUG
    debug << "ConstInitVal Target: " << target << ", Index: " << index << "\n";
#endif
    while (index < target)
    {
        vals.push_back(Operand());
        index++;
    }
#ifdef DEBUG
    debug << "ConstInitVal vals size: " << vals.size() << "\n";
#endif
    assert(index == target);
}

Operand VarDeclAST::Dump() const
{
#ifdef DEBUG
    debug << "VarDecl Dump\n";
#endif

    VarDefAST::base_type = b_type;
    for (int i = 0, n = var_defs.size(); i < n; ++i)
    {
        var_defs[i]->Dump();
    }
    return Operand();
}

BType VarDefAST::base_type = BType();

Operand VarDefAST::Dump() const
{
#ifdef DEBUG
    debug << "VarDef Dump\n";
#endif

    assert(type == IDENT || init_val.get());

    if (array_sizes.empty())
    {
        symbol_tables->RecordSymbol(ident, Symbol(Symbol::VAR));
        if (DeclAST::global)
        {
            Operand val = Operand().SetAsReturnMark();
            if (type == INITVAL)
            {
                val = init_val->Dump();
            }
            printer->GlobalAlloc(symbol_tables->GetName(ident), base_type, val);
            return Operand();
        }
        printer->Alloc(symbol_tables->GetName(ident), base_type, false);
        if (type == INITVAL)
        {
            Operand val = init_val->Dump();
            printer->Store(val, symbol_tables->GetName(ident), false);
        }
        return Operand();
    }

    std::deque<Operand> sizes;
    for (int i = 0, n = array_sizes.size(); i < n; ++i)
    {
        sizes.push_back(array_sizes[i]->Dump());
    }
    BType arr_type = BType(base_type.type, sizes);
    symbol_tables->RecordSymbol(ident, Symbol(Symbol::VAR_ARRAY, arr_type));

    if (type == IDENT)
    {
        if (DeclAST::global)
        {
            printer->GlobalAlloc(symbol_tables->GetName(ident), arr_type, std::deque<Operand>());
            return Operand();
        }
        printer->Alloc(symbol_tables->GetName(ident), arr_type, std::deque<Operand>());
        return Operand();
    }

    std::deque<int> steps({1});
    for (int i = sizes.size() - 1, step = 1; i >= 0; --i)
    {
        step *= sizes[i].ImmValue();
        steps.push_front(step);
    }

    std::deque<Operand> vals;
    int index = 0;
    ((InitValAST *)(init_val.get()))->Initialize(steps, vals, index, 0);
    if (DeclAST::global)
    {
        printer->GlobalAlloc(symbol_tables->GetName(ident), arr_type, vals);
    }
    else
    {
        printer->Alloc(symbol_tables->GetName(ident), arr_type, vals);
    }
    return Operand();
}

Operand InitValAST::Dump() const
{
#ifdef DEBUG
    debug << "InitVal Dump\n";
#endif

    assert(type == EXP);
    return exp->Dump();
}

void InitValAST::Initialize(const std::deque<int> &steps, std::deque<Operand> &vals, int &index, const int &dim) const
{
#ifdef DEBUG
    debug << "InitVal Initialize\n";
#endif

    assert(type == INITIALIZER || exp.get());

    if (type == EXP)
    {
        Operand value = exp->Dump();
        assert(!(DeclAST::global && value.IsReg()));
        vals.push_back(value);
        index++;
        return;
    }

    int steps_size = steps.size();
    assert(steps[steps_size - 1] == 1);

    int target = index + steps[dim];
    for (int i = 0, n = init_vals.size(); i < n; ++i)
    {
        int j = dim + 1;
        assert(j < steps_size);
        while (index % steps[j])
        {
            ++j;
        }
        ((InitValAST *)(init_vals[i].get()))->Initialize(steps, vals, index, j);
    }
#ifdef DEBUG
    debug << "InitVal Target: " << target << ", Index: " << index << "\n";
#endif
    while (index < target)
    {
        vals.push_back(Operand());
        index++;
    }
#ifdef DEBUG
    debug << "InitVal vals size: " << vals.size() << "\n";
#endif
    assert(index == target);
}

/*
=================================================
- Fuction Declaration
=================================================
*/

Operand FuncDefAST::Dump() const
{
#ifdef DEBUG
    debug << "FuncDef Dump\n";
#endif

    symbol_tables->RecordSymbol(ident, Symbol(Symbol::FUNC, func_type));
    symbol_tables->NewSymbolTable();
    FuncFParamAST::comma = false;
    DeclAST::global = false;
    BlockAST::new_symtab = false;
    printer->PreFunc(ident);
    for (int i = 0, n = params.size(), comma; i < n; ++i)
    {
        params[i]->Dump();
        FuncFParamAST::comma = true;
    }
    printer->PostFunc(func_type);
    printer->Label("entry");
    for (int i = 0, n = params.size(); i < n; ++i)
    {
        ((FuncFParamAST *)(params[i].get()))->Allocate();
    }
    Operand return_val = block->Dump();
    if (!return_val.IsReturnMark())
    {
        if (func_type.IsVoid())
        {
            printer->Ret(Operand().SetAsReturnMark());
        }
        else
        {
            printer->Ret(Operand());
        }
    }
    printer->EndCurBrac();
    DeclAST::global = true;
    symbol_tables->DeleteSymbolTable();
    return Operand();
}

bool FuncFParamAST::comma = false;

Operand FuncFParamAST::Dump() const
{
#ifdef DEBUG
    debug << "FuncFParam Dump\n";
#endif

    printer->FParam(ParamType(), ident, comma);
    return Operand();
}

Operand FuncFParamAST::Allocate() const
{
#ifdef DEBUG
    debug << "FuncFParam Allocate\n";
#endif

    if (is_array)
    {
        BType arr_type = ParamType();
        symbol_tables->RecordSymbol(ident, Symbol(Symbol::VAR_ARRAY, arr_type));
        std::string arr_name = symbol_tables->GetName(ident);
        printer->Alloc(arr_name, arr_type, false);
        printer->StoreFParam(arr_name, ident);
    }
    else
    {
        symbol_tables->RecordSymbol(ident, Symbol(Symbol::VAR));
        std::string val_name = symbol_tables->GetName(ident);
        printer->Alloc(val_name, type, false);
        printer->StoreFParam(val_name, ident);
    }
    return Operand();
}

BType FuncFParamAST::ParamType() const
{
    if (is_array)
    {
        std::deque<Operand> sizes({Operand()});
        for (int i = 0, n = array_sizes.size(); i < n; ++i)
        {
            sizes.push_back(array_sizes[i]->Dump());
        }
        return BType(type.type, sizes);
    }
    return type;
}

/*
=================================================
- Block
=================================================
*/

bool BlockAST::new_symtab = false;

Operand BlockAST::Dump() const
{
#ifdef DEBUG
    debug << "Block Dump\n";
#endif

    bool create_table = new_symtab;
    if (create_table)
    {
        symbol_tables->NewSymbolTable();
    }
    new_symtab = true;
    for (int i = 0, n = block_items.size(); i < n; ++i)
    {
        Operand return_val = block_items[i]->Dump();
        if (!return_val.IsNormal())
        {
            if (create_table)
            {
                symbol_tables->DeleteSymbolTable();
            }
            new_symtab = create_table;
            return return_val;
        }
    }
    if (create_table)
    {
        symbol_tables->DeleteSymbolTable();
    }
    new_symtab = create_table;
    return Operand();
}

Operand BlockItemAST::Dump() const
{
#ifdef DEBUG
    debug << "BlockItem Dump\n";
#endif

    return decl_or_stmt->Dump();
}

std::string StmtAST::branch_then = "then";

std::string StmtAST::branch_else = "else";

std::string StmtAST::branch_end = "end";

std::string StmtAST::loop_entry = "loop_entry";

std::string StmtAST::loop_body = "loop_body";

std::string StmtAST::loop_end = "loop_end";

Operand StmtAST::Dump() const
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
            printer->Ret(exp->Dump());
        }
        else
        {
            printer->Ret(Operand().SetAsReturnMark());
        }
        return Operand().SetAsReturnMark();
    }

    else if (type == LVAL)
    {
#ifdef DEBUG
        debug << "\tStmt - LVAL\n";
#endif
        std::string val_name = ((LValAST *)(lval_or_block.get()))->ident;
        Symbol symbol = symbol_tables->GetSymbol(val_name);
        Operand value = exp->Dump();
        if (symbol.type == Symbol::VAR)
        {
            printer->Store(value, symbol_tables->GetName(val_name), false);
            return Operand();
        }
        if (symbol.type == Symbol::VAR_ARRAY)
        {
            Operand elemptr = ((LValAST *)(lval_or_block.get()))->GetElemptr();
            printer->Store(value, elemptr);
            return Operand();
        }
        assert(false);
    }

    else if (type == EXP && exp)
    {
#ifdef DEBUG
        debug << "\tStmt - EXP\n";
#endif
        return exp->Dump();
    }

    else if (type == BLOCK)
    {
#ifdef DEBUG
        debug << "\tStmt - BLOCK\n";
#endif
        return lval_or_block->Dump();
    }

    else if (type == IF)
    {
#ifdef DEBUG
        debug << "\tStmt - IF\n";
#endif
        Operand cond = exp->Dump();
        if (!cond.IsReg())
        {
            if (cond.ImmValue())
            {
                return stmt->Dump();
            }
            else if (else_stmt)
            {
                return else_stmt->Dump();
            }
            return Operand();
        }

        int branch_mark = symbol_tables->NewBranchMark();
        std::string label_then = symbol_tables->Mark(branch_then, branch_mark);
        std::string label_else = symbol_tables->Mark(branch_else, branch_mark);
        std::string label_end = symbol_tables->Mark(branch_end, branch_mark);

        printer->Br(cond, label_then, (else_stmt ? label_else : label_end));

        printer->Label(label_then);
        Operand then_returned = stmt->Dump();
        if (then_returned.IsNormal())
        {
            printer->Jump(label_end);
        }

        Operand else_returned = Operand();
        if (else_stmt)
        {
            printer->Label(label_else);
            else_returned = else_stmt->Dump();
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
        Operand cond = exp->Dump();
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
        Operand return_val = stmt->Dump();
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

Operand ExpAST::Dump() const
{
#ifdef DEBUG
    debug << "Exp Dump\n";
#endif

    return lor_exp->Dump();
}

Operand LValAST::Dump() const
{
#ifdef DEBUG
    debug << "LVal Dump\n";
#endif

    Symbol symbol = symbol_tables->GetSymbol(ident);
    assert(array_indices.size() == 0 || symbol.type == Symbol::VAR_ARRAY || symbol.type == Symbol::CONST_ARRAY);
    if (symbol.type == Symbol::CONST)
    {
        return Operand(Operand::IMM, symbol.val);
    }
    if (symbol.type == Symbol::VAR)
    {
        Operand value = Operand(Operand::REG);
        printer->Load(value, symbol_tables->GetName(ident), false);
        return value;
    }
    if (symbol.type == Symbol::CONST_ARRAY || symbol.type == Symbol::VAR_ARRAY)
    {
        Operand elemptr = GetElemptr();
        int array_dim = symbol.ArrayType().ArraySizes().size();
        if (array_dim == array_indices.size())
        {
            Operand val = Operand(Operand::REG);
            printer->Load(val, elemptr);
            return val;
        }
        return elemptr;
    }
    assert(false);
    return Operand();
}

Operand LValAST::GetElemptr() const
{
#ifdef DEBUG
    debug << "LVal GetElemptr\n";
#endif

    Symbol symbol = symbol_tables->GetSymbol(ident);
    assert(symbol.type == Symbol::VAR_ARRAY || symbol.type == Symbol::CONST_ARRAY);
    BType arr_type = symbol.ArrayType();
    std::deque<int> arr_sizes = arr_type.ArraySizes();

    Operand elemptr = Operand(Operand::REG);
    if (array_indices.size() == 0)
    {
        if (arr_sizes[0])
        {
            printer->GetElemptr(elemptr, symbol_tables->GetName(ident), Operand());
        }
        else
        {
            Operand ptr = Operand(Operand::REG);
            printer->Load(ptr, symbol_tables->GetName(ident), false);
            printer->GetPtr(elemptr, ptr, Operand());
        }
        return elemptr;
    }

    for (int i = 0, n = array_indices.size(); i < n; ++i)
    {
        Operand index = array_indices[i]->Dump();
        if (i == 0)
        {
            if (arr_sizes[i])
            {
                printer->GetElemptr(elemptr, symbol_tables->GetName(ident), index);
            }
            else
            {
                Operand ptr = Operand(Operand::REG);
                printer->Load(ptr, symbol_tables->GetName(ident), false);
                printer->GetPtr(elemptr, ptr, index);
            }
            continue;
        }
        Operand new_elemptr = Operand(Operand::REG);
        printer->GetElemptr(new_elemptr, elemptr, index);
        elemptr = new_elemptr;
    }
    if (array_indices.size() < arr_sizes.size())
    {
        Operand new_elemptr = Operand(Operand::REG);
        printer->GetElemptr(new_elemptr, elemptr, Operand());
        elemptr = new_elemptr;
    }
    return elemptr;
}

Operand PrimaryExpAST::Dump() const
{
#ifdef DEBUG
    debug << "PrimaryExp Dump\n";
#endif

    return exp_or_lval_or_number->Dump();
}

Operand NumberAST::Dump() const
{
#ifdef DEBUG
    debug << "Number Dump\n";
#endif

    return Operand(Operand::IMM, number);
}

Operand UnaryExpAST::Dump() const
{
#ifdef DEBUG
    debug << "UnaryExp Dump\n";
#endif

    if (type == PRIMARY_EXP)
    {
        return primary_or_unary_exp->Dump();
    }
    if (type == CALL_FUNC)
    {
        std::deque<Operand> r_params;
        for (int i = 0, n = params.size(); i < n; ++i)
        {
            r_params.push_back(params[i]->Dump());
        }
        Symbol symbol = symbol_tables->GetSymbol(op_or_func);
        if (symbol.FuncRetType().IsVoid())
        {
            printer->Call(op_or_func, r_params, Operand().SetAsReturnMark());
            return Operand();
        }
        else
        {
            Operand ret_reg = Operand(Operand::REG);
            printer->Call(op_or_func, r_params, ret_reg);
            return ret_reg;
        }
    }
    Operand operand = primary_or_unary_exp->Dump();
    if (operand.IsReg())
    {
        Operand result = Operand(Operand::REG);
        switch (op_or_func[0])
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
        switch (op_or_func[0])
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

Operand MulExpAST::Dump() const
{
#ifdef DEBUG
    debug << "MulExp Dump\n";
#endif

    if (type == UNARY_EXP)
    {
        return unary_exp->Dump();
    }
    Operand left = mul_exp->Dump();
    Operand right = unary_exp->Dump();
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

Operand AddExpAST::Dump() const
{
#ifdef DEBUG
    debug << "AddExp Dump\n";
#endif

    if (type == MUL_EXP)
    {
        return mul_exp->Dump();
    }
    Operand left = add_exp->Dump();
    Operand right = mul_exp->Dump();
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

Operand RelExpAST::Dump() const
{
#ifdef DEBUG
    debug << "RelExp Dump\n";
#endif

    if (type == ADD_EXP)
    {
        return add_exp->Dump();
    }
    Operand left = rel_exp->Dump();
    Operand right = add_exp->Dump();
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

Operand EqExpAST::Dump() const
{
#ifdef DEBUG
    debug << "EqExp Dump\n";
#endif

    if (type == REL_EXP)
    {
        return rel_exp->Dump();
    }
    Operand left = eq_exp->Dump();
    Operand right = rel_exp->Dump();
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

Operand LAndExpAST::Dump() const
{
#ifdef DEBUG
    debug << "LAndExp Dump\n";
#endif

    if (type == EQ_EXP)
    {
        return eq_exp->Dump();
    }

    Operand left = land_exp->Dump();
    if (!left.IsReg())
    {
        if (left.ImmValue() == 0)
        {
            return Operand(Operand::IMM, 0);
        }

        Operand right = eq_exp->Dump();

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

    printer->Alloc(temp_result, BType::INT);
    printer->Ne(cond, left, Operand());
    printer->Br(cond, label_then, label_else);

    printer->Label(label_then);
    Operand right = eq_exp->Dump();
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

Operand LOrExpAST::Dump() const
{
#ifdef DEBUG
    debug << "LOrExp Dump\n";
#endif

    if (type == LAND_EXP)
    {
        return land_exp->Dump();
    }

    Operand left = lor_exp->Dump();
    if (!left.IsReg())
    {
        if (left.ImmValue())
        {
            return Operand(Operand::IMM, 1);
        }

        Operand right = land_exp->Dump();
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

    printer->Alloc(temp_result, BType::INT);
    printer->Ne(cond, left, Operand());
    printer->Br(cond, label_then, label_else);

    printer->Label(label_then);
    printer->Store(Operand(Operand::IMM, 1), temp_result);
    printer->Jump(label_end);

    printer->Label(label_else);
    Operand right = land_exp->Dump();
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

Operand ConstExpAST::Dump() const
{
#ifdef DEBUG
    debug << "ConstExp Dump\n";
#endif

    return exp->Dump();
}