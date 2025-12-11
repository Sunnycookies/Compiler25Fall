#include "ast.hpp"

std::string BaseAST::mode = MODE_DEBUG;

std::ostream &operator<<(std::ostream &os, const BaseAST &ast)
{
    ast.Dump(os);
    return os;
}

void CompUnitAST::Dump(std::ostream &os) const
{
    if (BaseAST::mode == MODE_DEBUG)
    {
        os << "CompUnitAST { ";
        func_def->Dump(os);
        os << " }";
        return;
    }
    else if (BaseAST::mode == MODE_KOOPA)
    {
        func_def->Dump(os);
        return;
    }
}

void FuncDefAST::Dump(std::ostream &os) const
{
    if (BaseAST::mode == MODE_DEBUG)
    {
        os << "FuncDefAST { ";
        func_type->Dump(os);
        os << ", " << ident << ", ";
        block->Dump(os);
        os << " }";
        return;
    }
    else if (BaseAST::mode == MODE_KOOPA)
    {
        os << "fun @" << ident << "(): ";
        func_type->Dump(os);
        os << " {\n";
        block->Dump(os);
        os << "}\n";
        return;
    }
}

void FuncTypeAST::Dump(std::ostream &os) const
{
    if (BaseAST::mode == MODE_DEBUG)
    {
        os << "FuncTypeAST { " << type << " }";
        return;
    }
    else if (BaseAST::mode == MODE_KOOPA)
    {
        if (type == "int")
        {
            os << "i32";
        }
        else if (type == "void")
        {
            os << "void";
        }
        return;
    }
}

void BlockAST::Dump(std::ostream &os) const
{
    if (BaseAST::mode == MODE_DEBUG)
    {
        os << "BlockAST { ";
        stmt->Dump(os);
        os << " }";
        return;
    }
    else if (BaseAST::mode == MODE_KOOPA)
    {
        os << "%entry:\n";
        stmt->Dump(os);
        return;
    }
}

void StmtAST::Dump(std::ostream &os) const
{
    if (BaseAST::mode == MODE_DEBUG)
    {
        os << "StmtAST { " << number << " }";
        return;
    }
    else if (BaseAST::mode == MODE_KOOPA)
    {
        os << "  ret " << number << "\n";
        return;
    }
}