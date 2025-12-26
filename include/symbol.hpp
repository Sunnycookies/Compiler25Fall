#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <stack>
#include <vector>
#include <cassert>
#include "defs.hpp"

class Symbol
{
public:
    enum symbol_type
    {
        CONST,
        VAR,
        FUNC,
    } type;
    int val;
    Symbol() = default;
    Symbol(const symbol_type &t, const int &v = 0);
    Symbol(const symbol_type &t, const BType::data_type &ret_type);
    BType::data_type FuncRetType();
};

class SymbolTables
{
private:
    static SymbolTables *pSymbolTables;
    typedef std::unordered_map<std::string, Symbol> symbol_table_t;
    typedef std::pair<std::string, int> local_symbol_t;
    typedef std::set<local_symbol_t> inner_symtab_t;
    std::vector<symbol_table_t> vert_symtabs;
    inner_symtab_t func_inner_syms;
    std::stack<int> loop_marks;
    int branch_count;
    SymbolTables() = default;
    ~SymbolTables() = delete;
    SymbolTables(const SymbolTables &) = delete;
    SymbolTables &operator=(const SymbolTables &) = delete;

public:
    static SymbolTables *GetSymbolTables();
    void NewSymbolTable(const bool &clear_local = false);
    void DeleteSymbolTable(const bool &clear_local = false);
    std::string Mark(const std::string &name, const int &mark);
    void RecordSymbol(const std::string &ident, const Symbol &value);
    Symbol GetSymbol(const std::string &ident);
    void LocalAllocate(const std::string &ident);
    bool LocalAllocated(const std::string &ident);
    int NewBranchMark();
    void PushLoop(const int &loop_mark);
    int GetTopLoop();
    void PopLoop();
};

extern SymbolTables *symbol_tables;