#pragma once

#include <cstdint>

class Moneybag
{
public:
    Moneybag() = delete;

    using coin_number_t = uint_least64_t;

private:
};

class Value
{
public:
    Value() = delete;
    ~Value() = default;
    explicit Value(Moneybag::coin_number_t const&);
    explicit Value(Value const&) = default;
    explicit Value(Value&&) = default;

    Value& operator= (Value const&) = default;
    Value& operator= (Value&&) = default;

private:
    Moneybag::coin_number_t value;
};

//constexpr Moneybag Livre = Moneybag(1, 0, 0);
//constexpr Moneybag Solidus = Moneybag(0, 1, 0);
//constexpr Moneybag Denier = Moneybag(0, 0, 1);