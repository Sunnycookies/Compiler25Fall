#pragma once

#include <string>
#include <unordered_map>

class Symbol
{
public:
    enum symbol_type
    {
        CONST,
        VAR,
    } type;
    int val;
    Symbol() = default;
    Symbol(const symbol_type &t, const int &v = 0);
};

class SymbolTable
{
private:
    static SymbolTable *pSymbolTable;
    std::unordered_map<std::string, Symbol> sym_table;
    SymbolTable() = default;
    ~SymbolTable() = delete;
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable &operator=(const SymbolTable &) = delete;

public:
    static SymbolTable *GetSymbolTable();
    int Record(const std::string &ident, const Symbol &value);
    bool Find(const std::string &ident);
    Symbol Get(const std::string &ident);
};

extern SymbolTable *symbol_table;