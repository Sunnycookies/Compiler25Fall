#pragma once

#include <memory>
#include <string>
#include <iostream>
#include "defs.hpp"

// 所有 AST 的基类
class BaseAST
{
public:
    static std::string mode;

    virtual ~BaseAST() = default;

    virtual void Dump(std::ostream &os) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const BaseAST &ast);
};

class CompUnitAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_def;

    void Dump(std::ostream &os) const override;
};

class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump(std::ostream &os) const override;
};

class FuncTypeAST : public BaseAST
{
public:
    std::string type;

    void Dump(std::ostream &os) const override;
};

class BlockAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> stmt;

    void Dump(std::ostream &os) const override;
};

class StmtAST : public BaseAST
{
public:
    int number;

    void Dump(std::ostream &os) const override;
};