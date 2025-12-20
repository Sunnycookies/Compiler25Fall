#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <stack>
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
    std::map<std::pair<std::string, int>, bool> sym_allocated;
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
    Symbol Get(const std::string &ident);
    std::string Mark(const std::string &name, const int &mark);
    void InnerBlockRecord(const std::string &ident, const Symbol &value);
    void InterBlockAllocate(const std::string &ident);
    bool InterBlockAllocated(const std::string &ident);
    int NewBranchMark();
    void PushLoop(const int &loop_mark);
    int GetTopLoop();
    void PopLoop();
};

extern SymbolTables *symbol_tables;