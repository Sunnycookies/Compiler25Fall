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
        CONST_ARRAY,
        VAR_ARRAY,
        FUNC,
    } type;
    int val;
    BType ret;
    Symbol();
    Symbol(const symbol_type &t, const int &v = 0);
    Symbol(const symbol_type &t, const BType &ret_type);
    Symbol(const Symbol &s);
    BType &FuncRetType();
};

class SymbolTables
{
private:
    static SymbolTables *pSymbolTables;
    typedef std::unordered_map<std::string, Symbol> symbol_table_t;
    std::vector<symbol_table_t> vert_symtabs;
    std::vector<int> sub_symtab_num;
    std::stack<int> loop_marks;
    int branch_count;
    SymbolTables() = default;
    ~SymbolTables() = delete;
    SymbolTables(const SymbolTables &) = delete;
    SymbolTables &operator=(const SymbolTables &) = delete;

public:
    static SymbolTables *GetSymbolTables();
    void NewSymbolTable();
    void DeleteSymbolTable();
    std::string Mark(const std::string &name, const int &mark);
    void RecordSymbol(const std::string &ident, const Symbol &value);
    std::string GetName(const std::string &ident);
    Symbol GetSymbol(const std::string &ident);
    int NewBranchMark();
    void PushLoop(const int &loop_mark);
    int GetTopLoop();
    void PopLoop();
};

extern SymbolTables *symbol_tables;