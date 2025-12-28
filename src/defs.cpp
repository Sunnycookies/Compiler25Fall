#include "defs.hpp"

#ifdef DEBUG
std::ostream &debug = std::cout;
#endif

BType::BType()
{
    type = VOID;
    array_base_type = nullptr;
    array_size = 0;
}

BType::BType(const data_type &t, std::deque<Operand> array_sizes)
{
    if (array_sizes.size())
    {
        type = ARRAY;
        array_size = array_sizes.front().ImmValue();
        array_sizes.pop_front();
        array_base_type = new BType(t, array_sizes);
    }
    else
    {
        type = t;
        array_size = 0;
        array_base_type = nullptr;
    }
}

BType::BType(const BType &t)
{
    type = t.type;
    array_size = t.array_size;
    if (type == ARRAY)
    {
        array_base_type = new BType(*(t.array_base_type));
    }
    else
    {
        array_base_type = nullptr;
    }
}

BType::~BType()
{
    if (array_base_type)
    {
        delete array_base_type;
    }
}

std::string BType::Short(const bool &colon) const
{
    std::string prefix = (colon ? ": " : "");
    if (type == VOID)
    {
        return "";
    }
    if (type == INT)
    {
        return prefix + "i32";
    }
    prefix += "*";
    BType *base = array_base_type;
    while (base->type == ARRAY)
    {
        prefix += "*";
        base = base->array_base_type;
    }
    if (base->type == VOID)
    {
        return "";
    }
    return prefix + base->Short(false);
}

BType &BType::operator=(const BType &t)
{
    type = t.type;
    array_size = t.array_size;
    if (type == ARRAY)
    {
        if (array_base_type)
        {
            delete array_base_type;
        }
        array_base_type = new BType(*(t.array_base_type));
    }
    else
    {
        array_base_type = nullptr;
    }
    return *this;
}

bool BType::IsVoid() const
{
    return type == VOID;
}

bool BType::IsInt() const
{
    return type == INT;
}

bool BType::IsArray() const
{
    return type == ARRAY;
}

std::deque<int> BType::ArraySizes() const
{
    assert(type == ARRAY);
    std::deque<int> arr_sizes({array_size});
    BType *current = array_base_type;
    while (current->type == ARRAY)
    {
        arr_sizes.push_back(current->array_size);
        current = current->array_base_type;
    }
    return arr_sizes;
}

bool BType::operator==(const BType &t) const
{
    if (type != t.type)
    {
        return false;
    }
    if (type == ARRAY)
    {
        if (array_size && t.array_size && (array_size != t.array_size))
        {
            return false;
        }
        return *array_base_type == *(t.array_base_type);
    }
    return true;
}

std::ostream &operator<<(std::ostream &os, const BType &type)
{
    switch (type.type)
    {
    case BType::INT:
        os << "i32";
        break;
    case BType::ARRAY:
        os << "[" << *(type.array_base_type) << ", " << type.array_size << "]";
        break;
    case BType::VOID:
    default:
        break;
    }
    return os;
}