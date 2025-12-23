#include "backend.hpp"

bool Backend::pass_entry = false;

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
    printer.SetOstream(os);
    Visit(raw_program.values);
    Visit(raw_program.funcs);
}

void Backend::Visit(const koopa_raw_slice_t &slice)
{
#ifdef DEBUG
    debug << "Visit Slice\n";
#endif

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
}

void Backend::Visit(const koopa_raw_function_t &func)
{
#ifdef DEBUG
    debug << "Visit Function\n";
#endif

    if (func->bbs.len == 0)
    {
        return;
    }

    printer.Text();
    printer.Global(func->name + 1);
    printer.Label(func->name + 1);

    int stack_size = 0;
    int temp_size = 0;
    int ra_size = 0;
    int local_size = 0;
    for (size_t i = 0; i < func->bbs.len; ++i)
    {
        auto bb = reinterpret_cast<koopa_raw_basic_block_t>(func->bbs.buffer[i]);
        local_size += bb->insts.len;
        for (size_t j = 0; j < bb->insts.len; ++j)
        {
            auto inst = reinterpret_cast<koopa_raw_value_t>(bb->insts.buffer[j]);
            if (inst->ty->tag == KOOPA_RTT_UNIT)
            {
                --local_size;
            }
            if (inst->kind.tag == KOOPA_RVT_CALL)
            {
                ra_size = 1;
                int param_num = inst->kind.data.call.args.len;
                temp_size = std::max(temp_size, param_num - 8);
            }
        }
    }

    ra_size *= DATA_SIZE;
    local_size *= DATA_SIZE;
    temp_size *= DATA_SIZE;
    stack_size = ra_size + local_size + temp_size;
    stack_size = (stack_size + ALIGN_SIZE - 1) / ALIGN_SIZE * ALIGN_SIZE;
    stack = Stack(stack_size, temp_size, ra_size);
    if (stack.GetStackSize())
    {
        printer.Addi(Register(Register::SP), Register(Register::SP), -stack_size);
    }
    if (stack.SaveRa())
    {
        printer.Sw(Register(Register::SP), Register(Register::RA), stack.RaOffset());
    }

    pass_entry = false;
    Visit(func->bbs);
}

void Backend::Visit(const koopa_raw_basic_block_t &bb)
{
#ifdef DEBUG
    debug << "Visit BasicBlock\n";
#endif
    if (pass_entry)
    {
        printer.Label(bb->name + 1);
    }
    pass_entry = true;
    Visit(bb->insts);
}

void Backend::Visit(const koopa_raw_value_t &value)
{
#ifdef DEBUG
    debug << "Visit Value\n";
#endif

    const auto &kind = value->kind;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        Visit(kind.data.ret);
        break;
    case KOOPA_RVT_BINARY:
        Visit(value, kind.data.binary);
        break;
    case KOOPA_RVT_STORE:
        Visit(kind.data.store);
        break;
    case KOOPA_RVT_LOAD:
        Visit(value, kind.data.load);
        break;
    case KOOPA_RVT_ALLOC:
        break;
    case KOOPA_RVT_BRANCH:
        Visit(kind.data.branch);
        break;
    case KOOPA_RVT_JUMP:
        Visit(kind.data.jump);
        break;
    case KOOPA_RVT_CALL:
        Visit(value, kind.data.call);
        break;
    case KOOPA_RVT_GLOBAL_ALLOC:
        Visit(value, kind.data.global_alloc);
        break;
    default:
        assert(false);
    }
}

void Backend::Visit(const koopa_raw_return_t &ret)
{
#ifdef DEBUG
    debug << "Visit Return\n";
#endif

    Register ret_reg = Register(Register::RET);
    if (ret.value)
    {
        if (ret.value->kind.tag == KOOPA_RVT_INTEGER)
        {
            printer.Li(ret_reg, ret.value->kind.data.integer.value);
        }
        else
        {
            printer.Lw(Register(Register::SP), ret_reg, stack.GetOffset(ret.value));
        }
    }
    if (stack.SaveRa())
    {
        printer.Lw(Register(Register::SP), Register(Register::RA), stack.RaOffset());
    }
    if (stack.GetStackSize())
    {
        printer.Addi(Register(Register::SP), Register(Register::SP), stack.GetStackSize());
    }
    printer.Ret();
    ret_reg.Unallocate();
}

void Backend::Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary)
{
#ifdef DEBUG
    debug << "Visit Binary\n";
#endif

    Register lhs_reg = Register(binary.lhs);
    Register rhs_reg = Register(binary.rhs);
    if (binary.lhs->kind.tag == KOOPA_RVT_INTEGER)
    {
        printer.Li(lhs_reg, binary.lhs->kind.data.integer.value);
    }
    else
    {
        printer.Lw(Register(Register::SP), lhs_reg, stack.GetOffset(binary.lhs));
    }
    if (binary.rhs->kind.tag == KOOPA_RVT_INTEGER)
    {
        printer.Li(rhs_reg, binary.rhs->kind.data.integer.value);
    }
    else
    {
        printer.Lw(Register(Register::SP), rhs_reg, stack.GetOffset(binary.rhs));
    }

    lhs_reg.Unallocate();
    rhs_reg.Unallocate();
    Register result_reg = Register(value);
    switch (binary.op)
    {
    case KOOPA_RBO_ADD:
        printer.Add(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_SUB:
        printer.Sub(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_MUL:
        printer.Mul(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_DIV:
        printer.Div(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_MOD:
        printer.Rem(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_LT:
        printer.Slt(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_LE:
        printer.Sgt(result_reg, lhs_reg, rhs_reg);
        printer.Seqz(result_reg, result_reg);
        break;
    case KOOPA_RBO_GT:
        printer.Sgt(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_GE:
        printer.Slt(result_reg, lhs_reg, rhs_reg);
        printer.Seqz(result_reg, result_reg);
        break;
    case KOOPA_RBO_EQ:
        printer.Xor(result_reg, lhs_reg, rhs_reg);
        printer.Seqz(result_reg, result_reg);
        break;
    case KOOPA_RBO_NOT_EQ:
        printer.Xor(result_reg, lhs_reg, rhs_reg);
        printer.Snez(result_reg, result_reg);
        break;
    case KOOPA_RBO_AND:
        printer.And(result_reg, lhs_reg, rhs_reg);
        break;
    case KOOPA_RBO_OR:
        printer.Or(result_reg, lhs_reg, rhs_reg);
        break;
    default:
        assert(false);
    }

    stack.Push(value, DATA_SIZE);
    printer.Sw(Register(Register::SP), result_reg, stack.GetOffset(value));
    result_reg.Unallocate();
}

void Backend::Visit(const koopa_raw_store_t &store)
{
#ifdef DEBUG
    debug << "Visit Store\n";
#endif

    Register dst = Register(store.dest);
    if (store.value->kind.tag == KOOPA_RVT_INTEGER)
    {
        printer.Li(dst, store.value->kind.data.integer.value);
    }
    else if (store.value->kind.tag == KOOPA_RVT_FUNC_ARG_REF)
    {
        int ind = store.value->kind.data.func_arg_ref.index;
        if (ind < Register::PARAM_REG_NUM)
        {
            Register param_reg = Register(Register::PARAM, ind);
            printer.Mv(dst, param_reg);
            param_reg.Unallocate();
        }
        else
        {
            int offset = stack.GetStackSize() + (ind - Register::PARAM_REG_NUM) * DATA_SIZE;
            printer.Lw(Register(Register::SP), dst, offset);
        }
    }
    else
    {
        printer.Lw(Register(Register::SP), dst, stack.GetOffset(store.value));
    }

    if (store.dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC)
    {
        Register addr = Register(store.dest);
        printer.La(addr, store.dest->name + 1);
        printer.Sw(addr, dst, 0);
        addr.Unallocate();
    }
    else
    {
        stack.Push(store.dest, DATA_SIZE);
        printer.Sw(Register(Register::SP), dst, stack.GetOffset(store.dest));
    }

    dst.Unallocate();
}

void Backend::Visit(const koopa_raw_value_t &value, const koopa_raw_load_t &load)
{
#ifdef DEBUG
    debug << "Visit Load\n";
#endif

    Register dst = Register(value);
    if (load.src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC)
    {
        Register addr = Register(load.src);
        printer.La(addr, load.src->name + 1);
        printer.Lw(addr, dst, 0);
    }
    else
    {
        printer.Lw(Register(Register::SP), dst, stack.GetOffset(load.src));
    }
    stack.Push(value, DATA_SIZE);
    printer.Sw(Register(Register::SP), dst, stack.GetOffset(value));
    dst.Unallocate();
}

void Backend::Visit(const koopa_raw_branch_t &branch)
{
#ifdef DEBUG
    debug << "Visit Branch\n";
#endif

    Register cond = Register(branch.cond);
    printer.Lw(Register(Register::SP), cond, stack.GetOffset(branch.cond));
    printer.Bnez(cond, branch.true_bb->name + 1);
    printer.J(branch.false_bb->name + 1);
    cond.Unallocate();
}

void Backend::Visit(const koopa_raw_jump_t &jump)
{
#ifdef DEBUG
    debug << "Visit Jump\n";
#endif

    printer.J(jump.target->name + 1);
}

void Backend::Visit(const koopa_raw_value_t &value, const koopa_raw_call_t &call)
{
#ifdef DEBUG
    debug << "Visit Call\n";
#endif

    std::vector<Register> param_regs;
    int param_num = call.args.len;
    for (int i = Register::PARAM_REG_NUM; i < param_num; ++i)
    {
        auto ptr = reinterpret_cast<koopa_raw_value_t>(call.args.buffer[i]);
        Register temp = Register(ptr);
        if (ptr->kind.tag == KOOPA_RVT_INTEGER)
        {
            printer.Li(temp, ptr->kind.data.integer.value);
        }
        else
        {
            printer.Lw(Register(Register::SP), temp, stack.GetOffset(ptr));
        }
        stack.Push(ptr, DATA_SIZE, true);
        printer.Sw(Register(Register::SP), temp, stack.GetOffset(ptr, true));
        temp.Unallocate();
    }
    for (int i = 0, n = std::min(param_num, Register::PARAM_REG_NUM); i < n; ++i)
    {
        Register param_reg = Register(Register::PARAM, i);
        auto ptr = reinterpret_cast<koopa_raw_value_t>(call.args.buffer[i]);
        if (ptr->kind.tag == KOOPA_RVT_INTEGER)
        {
            printer.Li(param_reg, ptr->kind.data.integer.value);
        }
        else
        {
            printer.Lw(Register(Register::SP), param_reg, stack.GetOffset(ptr));
        }
        param_regs.push_back(param_reg);
    }

    for (int i = 0, n = param_regs.size(); i < n; ++i)
    {
        param_regs[i].Unallocate();
    }
    for (int i = Register::PARAM_REG_NUM; i < param_num; ++i)
    {
        stack.Pop(DATA_SIZE, true);
    }

    printer.Call(call.callee->name + 1);
    if (value->ty->tag != KOOPA_RTT_UNIT)
    {
        Register ret_reg = Register(Register::RET);
        stack.Push(value, DATA_SIZE);
        printer.Sw(Register(Register::SP), ret_reg, stack.GetOffset(value));
        ret_reg.Unallocate();
    }
}

void Backend::Visit(const koopa_raw_value_t &value, const koopa_raw_global_alloc_t &global_alloc)
{
#ifdef DEBUG
    debug << "Visit Global Alloc\n";
#endif

    printer.Data();
    printer.Global(value->name + 1);
    printer.Label(value->name + 1);
    if (global_alloc.init->kind.tag == KOOPA_RVT_INTEGER)
    {
        printer.Word(global_alloc.init->kind.data.integer.value);
    }
    else if (global_alloc.init->kind.tag == KOOPA_RVT_ZERO_INIT)
    {
        printer.Zero(DATA_SIZE);
    }
}