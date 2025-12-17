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

int SymbolTable::Record(const std::string &ident, const Symbol &value)
{
    if (Find(ident))
    {
        return -1;
    }
    sym_table[ident] = value;
    return 0;
}

bool SymbolTable::Find(const std::string &ident)
{
    return sym_table.find(ident) != sym_table.end();
}

Symbol SymbolTable::Get(const std::string &ident)
{
    return sym_table[ident];
}