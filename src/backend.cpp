#include "backend.hpp"

Backend::Backend(const char *koopa_str)
{
    // 解析字符串 koopa_str, 得到 Koopa IR 程序
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(koopa_str, &program);
    assert(ret == KOOPA_EC_SUCCESS); // 确保解析时没有出错
    // 创建一个 raw program builder, 用来构建 raw program
    builder = koopa_new_raw_program_builder();
    // 将 Koopa IR 程序转换为 raw program
    raw_program = koopa_build_raw_program(builder, program);
    // 释放 Koopa IR 程序占用的内存
    koopa_delete_program(program);
}

Backend::~Backend()
{
    // 释放 raw program 占用的内存
    koopa_delete_raw_program_builder(builder);
}

std::ostream &operator<<(std::ostream &os, Backend &backend)
{
    backend.Visit(os);
    return os;
}

void Backend::Visit(std::ostream &os = std::cout)
{
    pos = &os;
    Visit(raw_program.values);
    *pos << "\t.text\n";
    Visit(raw_program.funcs);
}

Register Backend::Visit(const koopa_raw_slice_t &slice)
{
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        switch (slice.kind)
        {
        case KOOPA_RSIK_FUNCTION:
            Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
            break;
        case KOOPA_RSIK_BASIC_BLOCK:
            Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
            break;
        case KOOPA_RSIK_VALUE:
            Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
            break;
        default:
            assert(false);
        }
    }
    return Register();
}

Register Backend::Visit(const koopa_raw_function_t &func)
{
    *pos << "\t.global " << func->name + 1 << "\n";
    *pos << func->name + 1 << ":\n";
    Visit(func->bbs);
    return Register();
}

Register Backend::Visit(const koopa_raw_basic_block_t &bb)
{
    Visit(bb->insts);
    return Register();
}

Register Backend::Visit(const koopa_raw_return_t &ret)
{
    if (!ret.value)
    {
        *pos << "\tli a0, 0\n";
    }
    else if (ret.value->kind.tag == KOOPA_RVT_INTEGER)
    {
        *pos << "\tli a0, " << ret.value->kind.data.integer.value << "\n";
    }
    else
    {
        Register value_reg = Visit(ret.value);
        if (!value_reg.occupy_ret_reg())
        {
            *pos << "\tmv a0, " << value_reg << "\n";
        }
    }
    *pos << "\tret\n";
    return Register();
}

Register Backend::Visit(const koopa_raw_value_t &value)
{
    if (Register::allocated(value))
    {
        return Register(value, true);
    }
    const auto &kind = value->kind;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        return Visit(kind.data.ret);
    case KOOPA_RVT_INTEGER:
        return Visit(value, kind.data.integer);
    case KOOPA_RVT_BINARY:
        return Visit(value, kind.data.binary);
    default:
        assert(false);
    }
    return Register();
}

Register Backend::Visit(const koopa_raw_value_t &value, const koopa_raw_integer_t &integer)
{
    Register int_reg = Register();
    if (integer.value)
    {
        int_reg = Register(value);
        *pos << "\tli " << int_reg << ", " << integer.value << "\n";
    }
    return int_reg;
}

Register Backend::Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary)
{
    Register lhs_reg = Visit(binary.lhs);
    Register rhs_reg = Visit(binary.rhs);
    Register result_reg = Register(value);
    switch (binary.op)
    {
    case KOOPA_RBO_ADD:
        *pos << "\tadd " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_SUB:
        *pos << "\tsub " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_MUL:
        *pos << "\tmul " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_DIV:
        *pos << "\tdiv " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_MOD:
        *pos << "\trem " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_LT:
        *pos << "\tslt " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_LE:
        *pos << "\tsgt " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        *pos << "\tseqz " << result_reg << ", " << result_reg << "\n";
        break;
    case KOOPA_RBO_GT:
        *pos << "\tsgt " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_GE:
        *pos << "\tslt " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        *pos << "\tseqz " << result_reg << ", " << result_reg << "\n";
        break;
    case KOOPA_RBO_EQ:
        *pos << "\txor " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        *pos << "\tseqz " << result_reg << ", " << result_reg << "\n";
        break;
    case KOOPA_RBO_NOT_EQ:
        *pos << "\txor " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        *pos << "\tsnez " << result_reg << ", " << result_reg << "\n";
        break;
    case KOOPA_RBO_AND:
        *pos << "\tand " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    case KOOPA_RBO_OR:
        *pos << "\tor " << result_reg << ", " << lhs_reg << ", " << rhs_reg << "\n";
        break;
    default:
        assert(false);
    }
    lhs_reg.unallocate();
    rhs_reg.unallocate();
    return result_reg;
}
