#pragma once

#include <string>
#include <unordered_map>

class Symbol
{
public:
    enum symbol_type
    {
        LVAL,
    } type;
    int val;
    Symbol() = default;
    Symbol(const symbol_type &, const int & = 0);
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
    int record(const std::string &, const Symbol &);
    bool find(const std::string &);
    Symbol get(const std::string &);
};

extern SymbolTable *symbol_table;