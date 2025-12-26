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

void SymbolTables::NewSymbolTable()
{
    int current = sub_symtab_num.size();
    sub_symtab_num.push_back(0);
    if (current)
    {
        sub_symtab_num[current - 1]++;
    }
    vert_symtabs.push_back(symbol_table_t());
}

void SymbolTables::DeleteSymbolTable()
{
    sub_symtab_num.pop_back();
    vert_symtabs.pop_back();
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

std::string SymbolTables::GetName(const std::string &ident)
{
    int index;
    for (index = vert_symtabs.size() - 1; index >= 0; --index)
    {
        if (vert_symtabs[index].find(ident) != vert_symtabs[index].end())
        {
            break;
        }
    }
    assert(index >= 0);
    std::string name = ident + "_" + std::to_string(index);
    if (index)
    {
        name += "_" + std::to_string(sub_symtab_num[index - 1]);
    }
    return name;
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
        }
    }
    assert(false);
    return Symbol(Symbol::VAR, -1);
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