#include "stack.hpp"

Stack::Stack()
{
    capacity = 0;
    size = 0;
}

Stack::Stack(const int &cap)
{
    capacity = cap;
    size = 0;
}

int Stack::GetOffset(const koopa_raw_value_t &value)
{
    return value_off_map[value];
}

int Stack::GetStackSize()
{
    return capacity;
}

bool Stack::Find(const koopa_raw_value_t &value)
{
    return value_off_map.find(value) != value_off_map.end();
}

void Stack::Push(const koopa_raw_value_t &value, const int &type_size)
{
    if (Find(value))
    {
        return;
    }
    assert(size + type_size <= capacity);

#ifdef DEBUG
    debug << "Stack [" << size << "/" << capacity << "] -> [";
#endif

    value_off_map[value] = size;
    size += type_size;

#ifdef DEBUG
    debug << size << "/" << capacity << "]\n";
#endif
}