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
        pSymbolTables->returned = false;
    }
    return pSymbolTables;
}

void SymbolTables::NewSymbolTable()
{
    sym_tables.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTables::DeleteSymbolTable()
{
    sym_tables.pop_back();
}

void SymbolTables::Record(const std::string &ident, const Symbol &value)
{
    assert(sym_tables.back().find(ident) == sym_tables.back().end());
    sym_tables.back()[ident] = value;
}

Symbol SymbolTables::Get(const std::string &ident)
{
    for (int i = sym_tables.size() - 1; i >= 0; --i)
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
    assert(false);
    return Symbol(Symbol::VAR);
}

void SymbolTables::SetReturned()
{
    returned = true;
}

bool SymbolTables::IsReturned()
{
    return returned;
}