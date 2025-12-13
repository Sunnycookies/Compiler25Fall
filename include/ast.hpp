#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <cassert>
#include "defs.hpp"

class AsOperant
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
    AsOperant();
    AsOperant(const AsOperant &operant);
    AsOperant(const operant_type &t, const int &v);
    ~AsOperant();
    friend std::ostream &operator<<(std::ostream &os, const AsOperant &operant);
};

// 所有 AST 的基类
class BaseAST
{
public:
    static std::string mode;
    virtual ~BaseAST() = default;
    virtual AsOperant Dump(std::ostream &os) const = 0;
    friend std::ostream &operator<<(std::ostream &os, const BaseAST &ast);
};

class CompUnitAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_def;
    AsOperant Dump(std::ostream &os) const override;
};

class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;
    AsOperant Dump(std::ostream &os) const override;
};

class TypeAST : public BaseAST
{
public:
    std::string type;
    AsOperant Dump(std::ostream &os) const override;
};

class BlockAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> stmt;
    AsOperant Dump(std::ostream &os) const override;
};

class StmtAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    AsOperant Dump(std::ostream &os) const override;
};

class ExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> unary_exp;
    AsOperant Dump(std::ostream &os) const override;
};

class NumberAST : public BaseAST
{
public:
    int number;
    AsOperant Dump(std::ostream &os) const override;
};

class PrimaryExpAST : public BaseAST
{
public:
    enum primary_exp_type
    {
        EXP,
        NUMBER
    } type;
    std::unique_ptr<BaseAST> exp_or_number;
    AsOperant Dump(std::ostream &os) const override;
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
    AsOperant Dump(std::ostream &os) const override;
};