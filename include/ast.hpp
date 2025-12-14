#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <cassert>
#include "defs.hpp"

class AsOperand
{
public:
    enum operant_type
    {
        REG,
        IMM,
    } type;
    static int reg_count;
    union operant
    {
        int reg_no;
        int imm_value;
    } value;
    AsOperand();
    AsOperand(const AsOperand &);
    AsOperand(const operant_type &, const int &);
    ~AsOperand();
    friend std::ostream &operator<<(std::ostream &, const AsOperand &);
};

// 所有 AST 的基类
class BaseAST
{
public:
    static std::string mode;
    virtual ~BaseAST() = default;
    virtual AsOperand Dump(std::ostream &) const = 0;
    friend std::ostream &operator<<(std::ostream &, const BaseAST &);
};

class CompUnitAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_def;
    AsOperand Dump(std::ostream &) const override;
};

class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;
    AsOperand Dump(std::ostream &) const override;
};

class TypeAST : public BaseAST
{
public:
    std::string type;
    AsOperand Dump(std::ostream &) const override;
};

class BlockAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> stmt;
    AsOperand Dump(std::ostream &) const override;
};

class StmtAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    AsOperand Dump(std::ostream &) const override;
};

class ExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> lor_exp;
    AsOperand Dump(std::ostream &) const override;
};

class NumberAST : public BaseAST
{
public:
    int number;
    AsOperand Dump(std::ostream &) const override;
};

class PrimaryExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp_or_number;
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
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
    AsOperand Dump(std::ostream &) const override;
};