#include "symbol.hpp"

Symbol::Symbol(const symbol_type &t, const int &v)
{
    type = t;
    val = v;
}

SymbolTables *SymbolTables::pSymbolTables = nullptr;

SymbolTables *symbol_tables = SymbolTables::GetSymbolTables();

SymbolTables *SymbolTables::GetSymbolTables()
{
    if (pSymbolTables == nullptr)
    {
        pSymbolTables = new SymbolTables();
        pSymbolTables->current_table = -1;
        pSymbolTables->returned = false;
    }
    return pSymbolTables;
}

void SymbolTables::NewSymbolTable()
{
    sym_tables.push_back(std::unordered_map<std::string, Symbol>());
    ++current_table;
}

void SymbolTables::DeleteSymbolTable()
{
    sym_tables.pop_back();
    --current_table;
    // returned = false;
}

int SymbolTables::Record(const std::string &ident, const Symbol &value)
{
    assert(current_table >= 0);
    if (sym_tables[current_table].find(ident) != sym_tables[current_table].end())
    {
        return -1;
    }
    sym_tables[current_table][ident] = value;
    return 0;
}

Symbol SymbolTables::Get(const std::string &ident)
{
    for (int i = current_table; i >= 0; --i)
    {
        if (sym_tables[i].find(ident) != sym_tables[i].end())
        {
            Symbol sym = sym_tables[i][ident];
            if (sym.type == Symbol::CONST)
            {
                return sym;
            }
            else if (sym.type == Symbol::VAR)
            {
                return Symbol(Symbol::VAR, i + 1);
            }
        }
    }
    return Symbol(Symbol::CONST);
}

void SymbolTables::SetReturned()
{
    returned = true;
}

bool SymbolTables::IsReturned()
{
    return returned;
}