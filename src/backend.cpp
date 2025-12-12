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

std::ostream &operator<<(std::ostream &os, const Backend &backend)
{
    backend.Visit(os);
    return os;
}

void Backend::Visit(std::ostream &os = std::cout) const
{
    Visit(os, raw_program.values);
    os << "\t.text\n";
    Visit(os, raw_program.funcs);
}

void Backend::Visit(std::ostream &os, const koopa_raw_slice_t &slice) const
{
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        switch (slice.kind)
        {
        case KOOPA_RSIK_FUNCTION:
            Visit(os, reinterpret_cast<koopa_raw_function_t>(ptr));
            break;
        case KOOPA_RSIK_BASIC_BLOCK:
            Visit(os, reinterpret_cast<koopa_raw_basic_block_t>(ptr));
            break;
        case KOOPA_RSIK_VALUE:
            Visit(os, reinterpret_cast<koopa_raw_value_t>(ptr));
            break;
        default:
            assert(false);
        }
    }
}

void Backend::Visit(std::ostream &os, const koopa_raw_function_t &func) const
{
    os << "\t.global " << func->name + 1 << "\n";
    os << func->name + 1 << ":\n";
    Visit(os, func->bbs);
}

void Backend::Visit(std::ostream &os, const koopa_raw_basic_block_t &bb) const
{
    Visit(os, bb->insts);
}

void Backend::Visit(std::ostream &os, const koopa_raw_value_t &value) const
{
    const auto &kind = value->kind;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        Visit(os, kind.data.ret);
        break;
    case KOOPA_RVT_INTEGER:
        Visit(os, kind.data.integer);
        break;
    default:
        assert(false);
    }
}

void Backend::Visit(std::ostream &os, const koopa_raw_return_t &ret) const
{
    // 访问 return 指令的返回值
    if (ret.value)
    {
        os << "\tli a0, ";
        Visit(os, ret.value);
    }
    else
    {
        os << "\tli a0, 0";
    }
    os << "\n\tret";
}

void Backend::Visit(std::ostream &os, const koopa_raw_integer_t &integer) const
{
    os << integer.value;
}
