#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <cassert>
#include <deque>
#include "defs.hpp"
#include "operand.hpp"
#include "symbol.hpp"

// 所有 AST 的基类
class BaseAST
{
public:
    virtual ~BaseAST() = default;
    virtual Operand Dump(std::ostream &os) const = 0;
    friend std::ostream &operator<<(std::ostream &os, const BaseAST &ast);
};

/*
=================================================
- Program
=================================================
*/

class CompUnitAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_def;
    Operand Dump(std::ostream &os) const override;
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
    Operand Dump(std::ostream &os) const override;
};

class ConstDeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> b_type;
    std::deque<std::unique_ptr<BaseAST>> const_defs;
    Operand Dump(std::ostream &os) const override;
};

class BTypeAST : public BaseAST
{
public:
    std::string type;
    Operand Dump(std::ostream &os) const override;
};

class ConstDefAST : public BaseAST
{
public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    Operand Dump(std::ostream &os) const override;
};

class ConstInitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> const_exp;
    Operand Dump(std::ostream &os) const override;
};

class VarDeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> b_type;
    std::deque<std::unique_ptr<BaseAST>> var_defs;
    Operand Dump(std::ostream &os) const override;
};

class VarDefAST : public BaseAST
{
public:
    enum var_def_type
    {
        IDENT,
        INITVAL,
    } type;
    std::string ident;
    std::unique_ptr<BaseAST> init_val;
    Operand Dump(std::ostream &os) const override;
    Operand DumpWithType(std::ostream &os, const BaseAST &type) const;
};

class InitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    Operand Dump(std::ostream &os) const override;
};

/*
=================================================
- Fuction Declaration
=================================================
*/

class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;
    Operand Dump(std::ostream &os) const override;
};

class FuncTypeAST : public BaseAST
{
public:
    std::string type;
    Operand Dump(std::ostream &os) const override;
};

/*
=================================================
- Block
=================================================
*/

class BlockAST : public BaseAST
{
public:
    std::deque<std::unique_ptr<BaseAST>> block_items;
    Operand Dump(std::ostream &os) const override;
};

class BlockItemAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> decl_or_stmt;
    Operand Dump(std::ostream &os) const override;
};

class StmtAST : public BaseAST
{
public:
    enum stmt_type
    {
        LVAL,
        RETURN,
        EXP,
        BLOCK,
        IF,
    } type;
    std::unique_ptr<BaseAST> lval_or_block;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> else_stmt;
    Operand Dump(std::ostream &os) const override;
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
    Operand Dump(std::ostream &os) const override;
};

class LValAST : public BaseAST
{
public:
    std::string ident;
    Operand Dump(std::ostream &os) const override;
};

class PrimaryExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp_or_lval_or_number;
    Operand Dump(std::ostream &os) const override;
};

class NumberAST : public BaseAST
{
public:
    int number;
    Operand Dump(std::ostream &os) const override;
};

class UnaryExpAST : public BaseAST
{
public:
    enum unary_exp_type
    {
        PRIMARY_EXP,
        UNARY_EXP
    } type;
    std::string unary_op;
    std::unique_ptr<BaseAST> primary_or_unary_exp;
    Operand Dump(std::ostream &os) const override;
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
    Operand Dump(std::ostream &os) const override;
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
    Operand Dump(std::ostream &os) const override;
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
    Operand Dump(std::ostream &os) const override;
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
    Operand Dump(std::ostream &os) const override;
};

class LAndExpAST : public BaseAST
{
public:
    enum land_exp_type
    {
        EQ_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> land_exp;
    std::string land_op;
    std::unique_ptr<BaseAST> eq_exp;
    Operand Dump(std::ostream &os) const override;
};

class LOrExpAST : public BaseAST
{
public:
    enum lor_exp_type
    {
        LAND_EXP,
        BINARY_EXP
    } type;
    std::unique_ptr<BaseAST> lor_exp;
    std::string lor_op;
    std::unique_ptr<BaseAST> land_exp;
    Operand Dump(std::ostream &os) const override;
};

class ConstExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    Operand Dump(std::ostream &os) const override;
};