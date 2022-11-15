#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

namespace
{
    template<class T>
    constexpr auto overflow_add(T a, T b)
    {
        T tmp;
        if (__builtin_add_overflow(a, b, &tmp))
            throw std::out_of_range("Addition integer overflow");
        
        return tmp;
    }

    template<class T>
    constexpr auto overflow_mul(T a, T b)
    {
        T tmp;
        if (__builtin_mul_overflow(a, b, &tmp))
            throw std::out_of_range("Multiplication integer overflow");
        
        return tmp;
    }
}

class Moneybag
{
public:
    using coin_number_t = uint_least64_t;

    Moneybag() = delete;

    constexpr coin_number_t livre_number() const {return 0;};
    constexpr coin_number_t solidus_number() const {return 0;};
    constexpr coin_number_t denier_number() const {return 0;};
private:
};

class Value
{
public:
    Value() = delete;
    ~Value() = default;

    constexpr explicit Value(const Moneybag::coin_number_t& value)
        :value(value)
    {}

    constexpr explicit Value(const Moneybag& moneybag)
    {
        auto mul1 = overflow_mul(moneybag.livre_number(), static_cast<Moneybag::coin_number_t>(livre_to_denier));
        auto mul2 = overflow_mul(moneybag.solidus_number(), static_cast<Moneybag::coin_number_t>(solidus_to_denier));
        auto add = overflow_add(mul1, mul2);
        auto res = overflow_add(add, moneybag.denier_number());

        value = res;
    }

    constexpr explicit Value(const Value&) = default;
    constexpr explicit Value(Value&&) = default;

    constexpr Value& operator= (const Value&) = default;
    constexpr Value& operator= (Value&&) = default;

    auto operator<=>(const Value&) const = default;
    constexpr auto operator<=>(const Moneybag::coin_number_t& value) const
    {
        return this->value <=> value;
    }
    constexpr bool operator==(const Moneybag::coin_number_t& value) const
    {
        return this->value == value;
    }

    explicit operator std::string() const
    {
        std::string str = "";
        str.append("Value( ");
        str.append(std::to_string(value));
        str.append(" )");
        return str;
    };

private:
    static constexpr unsigned int solidus_to_denier = 12;
    static constexpr unsigned int livre_to_denier = 240;

    Moneybag::coin_number_t value;
};

//constexpr Moneybag Livre = Moneybag(1, 0, 0);
//constexpr Moneybag Solidus = Moneybag(0, 1, 0);
//constexpr Moneybag Denier = Moneybag(0, 0, 1);