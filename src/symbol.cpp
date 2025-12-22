#include "symbol.hpp"

Symbol::Symbol(const symbol_type &t, const int &v)
{
    type = t;
    val = v;
}

Symbol::Symbol(const symbol_type &t, const BType::data_type &ret_type)
{
    assert(t == FUNC);
    type = t;
    val = ret_type;
}

BType::data_type Symbol::FuncRetType()
{
    assert(type == FUNC);
    return BType::data_type(val);
}

SymbolTables *SymbolTables::pSymbolTables = nullptr;

SymbolTables *symbol_tables = SymbolTables::GetSymbolTables();

SymbolTables *SymbolTables::GetSymbolTables()
{
    if (pSymbolTables == nullptr)
    {
        pSymbolTables = new SymbolTables();
        pSymbolTables->branch_count = 0;
    }
    return pSymbolTables;
}

void SymbolTables::NewSymbolTable(const bool &clear_local)
{
    vert_symtabs.push_back(symbol_table_t());
    if (clear_local)
    {
        func_inner_syms.clear();
    }
}

void SymbolTables::DeleteSymbolTable(const bool &clear_local)
{
    vert_symtabs.pop_back();
    if (clear_local)
    {
        func_inner_syms.clear();
    }
}

std::string SymbolTables::Mark(const std::string &name, const int &mark)
{
    return name + "_" + std::to_string(mark);
}

void SymbolTables::RecordSymbol(const std::string &ident, const Symbol &value)
{
    assert(vert_symtabs.back().find(ident) == vert_symtabs.back().end());
    vert_symtabs.back()[ident] = value;
}

Symbol SymbolTables::GetSymbol(const std::string &ident)
{
    for (int i = vert_symtabs.size() - 1; i >= 0; --i)
    {
        if (vert_symtabs[i].find(ident) != vert_symtabs[i].end())
        {
            Symbol sym = vert_symtabs[i][ident];
            if (sym.type == Symbol::CONST || sym.type == Symbol::FUNC)
            {
                return sym;
            }
            else if (sym.type == Symbol::VAR)
            {
                return Symbol(Symbol::VAR, i);
            }
            else if (sym.type == Symbol::PARAM)
            {
                return Symbol(Symbol::PARAM, i);
            }
        }
    }
    assert(false);
    return Symbol(Symbol::VAR, -1);
}

void SymbolTables::LocalAllocate(const std::string &ident)
{
    func_inner_syms.insert(local_symbol_t(ident, vert_symtabs.size()));
}

bool SymbolTables::LocalAllocated(const std::string &ident)
{
    return func_inner_syms.count(local_symbol_t(ident, vert_symtabs.size()));
}

int SymbolTables::NewBranchMark()
{
    return ++branch_count;
}

void SymbolTables::PushLoop(const int &loop_mark)
{
    loop_marks.push(loop_mark);
}

int SymbolTables::GetTopLoop()
{
    assert(loop_marks.size());
    return loop_marks.top();
}

void SymbolTables::PopLoop()
{
    assert(loop_marks.size());
    loop_marks.pop();
}