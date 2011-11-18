#include "interface/Input.h"

bool operator <(const Input &l, const Input &r)
{
    return l.type() < r.type();
}

bool operator >(const Input &l, const Input &r)
{
    return l.type() > r.type();
}

std::ostream &operator <<(std::ostream &out, const Input &input)
{
    return out << static_cast<std::string>(input);
}
