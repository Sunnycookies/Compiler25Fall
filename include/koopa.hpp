#pragma once

#include <iostream>
#include <string>
#include <deque>
#include "operand.hpp"
#include "defs.hpp"

class KoopaCode
{
private:
    std::ostream *pos;
    void PrintArray(const std::deque<int> &arr_sizes, const std::deque<Operand> &init_vals, int &index, const int &dim);

public:
    KoopaCode(std::ostream &os = std::cout);
    void SetOstream(std::ostream &os);
    void EndCurBrac();
    void DeclFunc(const std::string &ident, const std::deque<BType> &ret_and_params);
    void PreFunc(const std::string &func);
    void PostFunc(const BType &type);
    void FParam(const BType &type, const std::string &ident, const bool &comma = false);
    void StoreFParam(const std::string &temp, const std::string &fparam);
    void Label(const std::string &label);
    void Alloc(const std::string &var, const BType &type, const bool &temp = true);
    void Alloc(const std::string &arr, const BType &type, const std::deque<Operand> &init_vals);
    void GlobalAlloc(const std::string &var, const BType &type, const Operand &v);
    void GlobalAlloc(const std::string &arr, const BType &type, const std::deque<Operand> &init_vals);
    void Store(const Operand &reg_or_imm, const std::string &var, const bool &temp = true);
    void Store(const Operand &reg_or_imm, const Operand &elemptr);
    void Load(const Operand &reg, const std::string &var, const bool &temp = true);
    void Load(const Operand &reg, const Operand &elemptr);
    void GetElemptr(const Operand &elemptr, const std::string &arr, const Operand &index);
    void GetElemptr(const Operand &elemptr, const Operand &base_ptr, const Operand &index);
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