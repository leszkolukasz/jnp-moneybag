#include "moneybag.h"

#include <stdexcept>

namespace
{
    template<class T>
    auto overflow_add(T a, T b)
    {
        T tmp;
        if (__builtin_add_overflow(a, b, &tmp))
            throw std::out_of_range("Addition integer overflow");
        
        return tmp;
    }

    template<class T>
    auto overflow_mul(T a, T b)
    {
        T tmp;
        if (__builtin_mul_overflow(a, b, &tmp))
            throw std::out_of_range("Multiplication integer overflow");
        
        return tmp;
    }
}

Value::Value(const Moneybag::coin_number_t& value)
    :value(value)
{}

Value::Value(Moneybag const& moneybag)
{
    auto mul1 = overflow_mul(moneybag.livre_number(), static_cast<Moneybag::coin_number_t>(livre_to_denier));
    auto mul2 = overflow_mul(moneybag.solidus_number(), static_cast<Moneybag::coin_number_t>(solidus_to_denier));
    auto add = overflow_add(mul1, mul2);
    auto res = overflow_add(add, moneybag.denier_number());

    value = res;
}