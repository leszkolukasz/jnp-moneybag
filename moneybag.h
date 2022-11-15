#pragma once

#include <cstdint>
#include <string>

class Moneybag
{
public:
    using coin_number_t = uint_least64_t;

    Moneybag() = delete;

    coin_number_t livre_number() const;
    coin_number_t solidus_number() const;
    coin_number_t denier_number() const;
private:
};

class Value
{
public:
    Value() = delete;
    ~Value() = default;
    explicit Value(const Moneybag::coin_number_t&);
    explicit Value(const Moneybag&);
    explicit Value(const Value&) = default;
    explicit Value(Value&&) = default;

    Value& operator= (const Value&) = default;
    Value& operator= (Value&&) = default;

    auto operator<=>(const Value&) const = default;

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