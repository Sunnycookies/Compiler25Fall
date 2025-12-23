#include "stack.hpp"

Stack::Stack()
{
    total_capacity = 0;
    local_capacity = 0;
    ra_capacity = 0;
    temp_capacity = 0;
    temp_offset = 0;
    local_offset = 0;
}

Stack::Stack(const int &total_cap, const int &temp_cap, const int &ra_cap)
{
    total_capacity = total_cap;
    ra_capacity = ra_cap;
    local_capacity = total_cap - temp_cap - ra_cap;
    temp_capacity = temp_cap;
#ifdef DEBUG
    debug << "Stack Init:\n";
    debug << "\ttotal: " << total_capacity << "\n";
    debug << "\tra: " << ra_capacity << "\n";
    debug << "\tlocal: " << local_capacity << "\n";
    debug << "\ttemp: " << temp_capacity << "\n";
#endif
    local_offset = 0;
    temp_offset = 0;
    local_off_map = offset_map_t();
    temp_off_map = offset_map_t();
}

int Stack::GetOffset(const koopa_raw_value_t &value, const bool &temp)
{
    if (temp)
    {
        return temp_off_map[value];
    }
    else
    {
        return local_off_map[value];
    }
}

int Stack::GetStackSize()
{
    return total_capacity;
}

bool Stack::Find(const koopa_raw_value_t &value)
{
    return local_off_map.find(value) != local_off_map.end();
}

void Stack::Push(const koopa_raw_value_t &value, const int &type_size, const bool &temp)
{

    if (!temp && Find(value))
    {
        return;
    }

    if (temp)
    {
        assert(temp_offset + type_size <= temp_capacity);
        temp_off_map[value] = temp_offset;
        temp_offset += type_size;
    }
    else
    {
        assert(local_offset + type_size <= local_capacity);
        local_off_map[value] = local_offset + temp_capacity;
        local_offset += type_size;
    }
}

void Stack::Pop(const int &type_size, const bool &temp)
{
    temp_offset -= type_size * temp;
    local_offset -= type_size * (1 - temp);
}

bool Stack::SaveRa()
{
    return ra_capacity;
}

int Stack::RaOffset()
{
    return total_capacity - ra_capacity;
}