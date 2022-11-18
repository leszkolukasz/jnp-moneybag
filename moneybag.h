#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

namespace
{
    template<class T>
    constexpr auto overflow_sub(T a, T b)
    {
        T tmp;
        if (__builtin_sub_overflow(a, b, &tmp))
            throw std::out_of_range("Subtraction integer overflow");

        return tmp;
    }

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

    constexpr Moneybag() = delete;
    constexpr Moneybag(uint_least64_t livre, uint_least64_t solidus,
                       uint_least64_t denier) : livre(livre), solidus(solidus),
                       denier(denier) {};
    constexpr Moneybag(const Moneybag&) = default;
    constexpr ~Moneybag() = default;

    constexpr Moneybag& operator= (const Moneybag&) = default;
    //constexpr Moneybag& operator= (Moneybag&&) = default;

    constexpr Moneybag operator+ (const Moneybag& other) const {
        return {overflow_add(livre, other.livre),
                overflow_add(solidus, other.solidus),
                overflow_add(denier, other.denier)};
    }
    constexpr Moneybag operator- (const Moneybag& other) const {
        if(livre < other.livre || solidus < other.solidus ||
           denier < other.denier) {
            throw std::out_of_range("Illegal subtraction");
        }
        return {overflow_sub(livre, other.livre),
                overflow_sub(solidus, other.solidus),
                overflow_sub(denier, other.denier)};
    }
    constexpr Moneybag operator* (uint_least64_t multiplier) const {
        return {overflow_mul(livre, multiplier),
                overflow_mul(solidus, multiplier),
                overflow_mul(denier, multiplier)};
    }

    constexpr Moneybag& operator+= (const Moneybag& other) {
        coin_number_t newLivre = overflow_add(livre, other.livre);
        coin_number_t newSolidus = overflow_add(solidus, other.solidus);
        coin_number_t newDenier = overflow_add(denier, other.denier);
        livre = newLivre;
        solidus = newSolidus;
        denier = newDenier;
        return *this;
    }
    constexpr Moneybag& operator-= (const Moneybag& other) {
        if(livre < other.livre || solidus < other.solidus ||
           denier < other.denier) {
            throw std::out_of_range("Illegal subtraction");
        }
        coin_number_t newLivre = overflow_sub(livre, other.livre);
        coin_number_t newSolidus = overflow_sub(solidus, other.solidus);
        coin_number_t newDenier = overflow_sub(denier, other.denier);
        livre = newLivre;
        solidus = newSolidus;
        denier = newDenier;
        return *this;
    }
    constexpr Moneybag& operator*= (uint_least64_t multiplier) {
        coin_number_t newLivre = overflow_mul(livre, multiplier);
        coin_number_t newSolidus = overflow_mul(solidus, multiplier);
        coin_number_t newDenier = overflow_mul(denier, multiplier);
        livre = newLivre;
        solidus = newSolidus;
        denier = newDenier;
        return *this;
    }

    constexpr auto operator<=> (const Moneybag& other) const {
        if(livre < other.livre && solidus < other.solidus &&
            denier < other.denier) {
            return std::partial_ordering::less;
        } else if(livre == other.livre && solidus == other.solidus &&
                    denier == other.denier) {
            return std::partial_ordering::equivalent;
        } else if(livre > other.livre && solidus > other.solidus &&
                    denier > other.denier) {
            return std::partial_ordering::greater;
        } else {
            return std::partial_ordering::unordered;
        }
    }

    constexpr bool operator== (const Moneybag& other) const {
        return (livre == other.livre && solidus == other.solidus &&
                denier == other.denier);
    }

    explicit operator bool() const {
        return livre != 0 || solidus != 0 || denier != 0;
    };

    constexpr coin_number_t livre_number() const {return livre;};
    constexpr coin_number_t solidus_number() const {return solidus;};
    constexpr coin_number_t denier_number() const {return denier;};
private:
    coin_number_t livre, solidus, denier;

    friend std::ostream & operator<<(std::ostream &os, const Moneybag& p) {
        return os << "(" + std::to_string(p.livre) +
                     (p.livre == 1 ? " livr, " : " livres, ") +
                     std::to_string(p.solidus) +
                     (p.solidus == 1 ? " solidus, " : " soliduses, ") +
                     std::to_string(p.denier) +
                     (p.denier == 1 ? " denier)" : " deniers)");
    }
};

constexpr Moneybag operator* (uint_least64_t multiplier, const Moneybag& x) {
    return x * multiplier;
}

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

constexpr Moneybag Livre = Moneybag(1, 0, 0);
constexpr Moneybag Solidus = Moneybag(0, 1, 0);
constexpr Moneybag Denier = Moneybag(0, 0, 1);