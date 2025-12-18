#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

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

class SymbolTables
{
private:
    static SymbolTables *pSymbolTables;
    std::vector<std::unordered_map<std::string, Symbol>> sym_tables;
    int current_table;
    bool returned;
    SymbolTables() = default;
    ~SymbolTables() = delete;
    SymbolTables(const SymbolTables &) = delete;
    SymbolTables &operator=(const SymbolTables &) = delete;

public:
    static SymbolTables *GetSymbolTables();
    void NewSymbolTable();
    void DeleteSymbolTable();
    int Record(const std::string &ident, const Symbol &value);
    Symbol Get(const std::string &ident);
    void SetReturned();
    bool IsReturned();
};

extern SymbolTables *symbol_tables;