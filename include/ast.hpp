#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <cassert>
#include <deque>
#include "defs.hpp"
#include "operand.hpp"
#include "symbol.hpp"
#include "koopa.hpp"

// 所有 AST 的基类
class BaseAST
{
public:
    static KoopaCode *printer;
    virtual ~BaseAST() = default;
    virtual Operand Dump() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const BaseAST &ast);
};

/*
=================================================
- Program
=================================================
*/

class ProgramAST : public BaseAST
{
public:
    std::deque<std::unique_ptr<BaseAST>> comp_units;
    Operand Dump() const override;
};

class CompUnitAST : public BaseAST
{
public:
    enum comp_unit_type
    {
        FUNC,
        DECL,
    } type;
    std::unique_ptr<BaseAST> func_def_or_decl;
    Operand Dump() const override;
};

/*
=================================================
- Variable Declaration & Definition
=================================================
*/

class DeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> const_or_var_decl;
    Operand Dump() const override;
};

class ConstDeclAST : public BaseAST
{
public:
    BType::data_type type;
    std::deque<std::unique_ptr<BaseAST>> const_defs;
    Operand Dump() const override;
};

class ConstDefAST : public BaseAST
{
public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    Operand Dump() const override;
};

class ConstInitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> const_exp;
    Operand Dump() const override;
};

class VarDeclAST : public BaseAST
{
public:
    BType::data_type b_type;
    std::deque<std::unique_ptr<BaseAST>> var_defs;
    Operand Dump() const override;
};

class VarDefAST : public BaseAST
{
public:
    static BType::data_type current_type;
    enum var_def_type
    {
        IDENT,
        INITVAL,
    } type;
    std::string ident;
    std::unique_ptr<BaseAST> init_val;
    Operand Dump() const override;
};

class InitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    Operand Dump() const override;
};

/*
=================================================
- Fuction Declaration & Definitino
=================================================
*/

class FuncDefAST : public BaseAST
{
public:
    BType::data_type func_type;
    std::string ident;
    std::deque<std::unique_ptr<BaseAST>> params;
    std::unique_ptr<BaseAST> block;
    Operand Dump() const override;
};

class FuncFParamAST : public BaseAST
{
public:
    static bool comma;
    BType::data_type type;
    std::string ident;
    Operand Dump() const override;
    Operand Allocate() const;
};

/*
=================================================
- Block
=================================================
*/

class BlockAST : public BaseAST
{
public:
    static bool new_symbol_table;
    std::deque<std::unique_ptr<BaseAST>> block_items;
    Operand Dump() const override;
};

class BlockItemAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> decl_or_stmt;
    Operand Dump() const override;
};

class StmtAST : public BaseAST
{
private:
    static std::string branch_then;
    static std::string branch_else;
    static std::string branch_end;
    static std::string loop_entry;
    static std::string loop_body;
    static std::string loop_end;

public:
    enum stmt_type
    {
        LVAL,
        RETURN,
        EXP,
        BLOCK,
        IF,
        WHILE,
        BREAK,
        CONTINUE,
    } type;
    std::unique_ptr<BaseAST> lval_or_block;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> else_stmt;
    Operand Dump() const override;
};

/*
=================================================
- Expression
=================================================
*/

class ExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> lor_exp;
    Operand Dump() const override;
};

class LValAST : public BaseAST
{
public:
    std::string ident;
    Operand Dump() const override;
};

class PrimaryExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp_or_lval_or_number;
    Operand Dump() const override;
};

class NumberAST : public BaseAST
{
public:
    int number;
    Operand Dump() const override;
};

class UnaryExpAST : public BaseAST
{
public:
    enum unary_exp_type
    {
        PRIMARY_EXP,
        UNARY_EXP,
        CALL_FUNC,
    } type;
    std::string op_or_func;
    std::unique_ptr<BaseAST> primary_or_unary_exp;
    std::deque<std::unique_ptr<BaseAST>> params;
    Operand Dump() const override;
};

class MulExpAST : public BaseAST
{
public:
    enum mul_exp_type
    {
        UNARY_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> mul_exp;
    std::string mul_op;
    std::unique_ptr<BaseAST> unary_exp;
    Operand Dump() const override;
};

class AddExpAST : public BaseAST
{
public:
    enum add_exp_type
    {
        MUL_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> add_exp;
    std::string add_op;
    std::unique_ptr<BaseAST> mul_exp;
    Operand Dump() const override;
};

class RelExpAST : public BaseAST
{
public:
    enum rel_exp_type
    {
        ADD_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> rel_exp;
    std::string rel_op;
    std::unique_ptr<BaseAST> add_exp;
    Operand Dump() const override;
};

class EqExpAST : public BaseAST
{
public:
    enum eq_exp_type
    {
        REL_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> eq_exp;
    std::string eq_op;
    std::unique_ptr<BaseAST> rel_exp;
    Operand Dump() const override;
};

class LAndExpAST : public BaseAST
{
private:
    static std::string and_then;
    static std::string and_else;
    static std::string and_end;
    static std::string and_temp;

public:
    enum land_exp_type
    {
        EQ_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> land_exp;
    std::string land_op;
    std::unique_ptr<BaseAST> eq_exp;
    Operand Dump() const override;
};

class LOrExpAST : public BaseAST
{
private:
    static std::string or_then;
    static std::string or_else;
    static std::string or_end;
    static std::string or_temp;

public:
    enum lor_exp_type
    {
        LAND_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> lor_exp;
    std::string lor_op;
    std::unique_ptr<BaseAST> land_exp;
    Operand Dump() const override;
};

class ConstExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    Operand Dump() const override;
};