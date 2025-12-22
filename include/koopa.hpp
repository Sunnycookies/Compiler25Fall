#pragma once

#include <iostream>
#include <string>
#include "symbol.hpp"
#include "operand.hpp"
#include "deque"

class KoopaCode
{
private:
    std::ostream *pos;

public:
    KoopaCode(std::ostream &os = std::cout);
    void SetOstream(std::ostream &os);
    void EndCurBrac();
    void Type(const BType::data_type &type);
    void Label(const std::string &label);
    void PreFunc(const std::string &func);
    void FParam(const BType::data_type &type, const std::string &ident, const bool &comma = false);
    void StoreFParam(const std::string &temp, const std::string &fparam);
    void PostFunc(const BType::data_type &type);
    void Alloc(const std::string &var, const BType::data_type &type, const bool &temp = true);
    void Store(const Operand &reg_or_imm, const std::string &var, const bool &temp = true);
    void Load(const Operand &reg, const std::string &var, const bool &temp = true);
    void Br(const Operand &cond, const std::string &t_label, const std::string &f_label);
    void Jump(const std::string &label);
    void Add(const Operand &dst, const Operand &src1, const Operand &src2);
    void Sub(const Operand &dst, const Operand &src1, const Operand &src2);
    void Mul(const Operand &dst, const Operand &src1, const Operand &src2);
    void Div(const Operand &dst, const Operand &src1, const Operand &src2);
    void Mod(const Operand &dst, const Operand &src1, const Operand &src2);
    void Lt(const Operand &dst, const Operand &src1, const Operand &src2);
    void Gt(const Operand &dst, const Operand &src1, const Operand &src2);
    void Le(const Operand &dst, const Operand &src1, const Operand &src2);
    void Ge(const Operand &dst, const Operand &src1, const Operand &src2);
    void Eq(const Operand &dst, const Operand &src1, const Operand &src2);
    void Ne(const Operand &dst, const Operand &src1, const Operand &src2);
    void And(const Operand &dst, const Operand &src1, const Operand &src2);
    void Or(const Operand &dst, const Operand &src1, const Operand &src2);
    void Ret(const Operand &reg_or_imm);
    void Call(const std::string &func, std::deque<Operand> &params, const Operand &ret);
};