#include "symbol.hpp"

Symbol::Symbol(const symbol_type &t, const int &v)
{
    type = t;
    val = v;
}

SymbolTable *SymbolTable::pSymbolTable = nullptr;

SymbolTable* symbol_table = SymbolTable::GetSymbolTable();

SymbolTable *SymbolTable::GetSymbolTable()
{
    if (pSymbolTable == nullptr)
    {
        pSymbolTable = new SymbolTable();
    }
    return pSymbolTable;
}

int SymbolTable::record(const std::string &ident, const Symbol &value)
{
    if (find(ident))
    {
        return -1;
    }
    sym_table[ident] = value;
    return 0;
}

bool SymbolTable::find(const std::string &ident)
{
    return sym_table.find(ident) != sym_table.end();
}

Symbol SymbolTable::get(const std::string &ident)
{
    return sym_table[ident];
}