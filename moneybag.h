#ifndef MONEYBAG_H
#define MONEYBAG_H

#include <cstdint>
#include <stdexcept>
#include <string>

namespace
{
template <class T> constexpr auto overflow_add(T a, T b)
{
    T tmp;
    if (__builtin_add_overflow(a, b, &tmp))
        throw std::out_of_range("Addition integer overflow");

    return tmp;
}

template <class T> constexpr auto overflow_sub(T a, T b)
{
    T tmp;
    if (__builtin_sub_overflow(a, b, &tmp))
        throw std::out_of_range("Subtraction integer overflow");

    return tmp;
}

template <class T> constexpr auto overflow_mul(T a, T b)
{
    T tmp;
    if (__builtin_mul_overflow(a, b, &tmp))
        throw std::out_of_range("Multiplication integer overflow");

    return tmp;
}
} // namespace

class Moneybag
{
  public:
    using coin_number_t = uint_least64_t;

    constexpr Moneybag() = delete;
    constexpr Moneybag(coin_number_t livre, coin_number_t solidus, coin_number_t denier)
        : livre(livre), solidus(solidus), denier(denier){};
    constexpr Moneybag(const Moneybag &) = default;
    ~Moneybag() = default;

    constexpr Moneybag &operator=(const Moneybag &) = default;

    constexpr const Moneybag operator+(const Moneybag &other) const
    {
        return Moneybag(*this) += other;
    }

    constexpr const Moneybag operator-(const Moneybag &other) const
    {
        return Moneybag(*this) -= other;
    }

    constexpr const Moneybag operator*(coin_number_t multiplier) const
    {
        return Moneybag(*this) *= multiplier;
    }

    constexpr Moneybag &operator+=(const Moneybag &other)
    {
        coin_number_t newLivre = overflow_add(livre, other.livre);
        coin_number_t newSolidus = overflow_add(solidus, other.solidus);
        coin_number_t newDenier = overflow_add(denier, other.denier);
        livre = newLivre;
        solidus = newSolidus;
        denier = newDenier;
        return *this;
    }

    constexpr Moneybag &operator-=(const Moneybag &other)
    {
        if (livre < other.livre || solidus < other.solidus || denier < other.denier)
        {
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

    constexpr Moneybag &operator*=(coin_number_t multiplier)
    {
        coin_number_t newLivre = overflow_mul(livre, multiplier);
        coin_number_t newSolidus = overflow_mul(solidus, multiplier);
        coin_number_t newDenier = overflow_mul(denier, multiplier);
        livre = newLivre;
        solidus = newSolidus;
        denier = newDenier;
        return *this;
    }

    constexpr auto operator<=>(const Moneybag &other) const
    {
        if (livre == other.livre && solidus == other.solidus && denier == other.denier)
            return std::partial_ordering::equivalent;

        else if (livre <= other.livre && solidus <= other.solidus && denier <= other.denier)
            return std::partial_ordering::less;

        else if (livre >= other.livre && solidus >= other.solidus && denier >= other.denier)
            return std::partial_ordering::greater;

        return std::partial_ordering::unordered;
    }

    constexpr bool operator==(const Moneybag &other) const
    {
        return (livre == other.livre && solidus == other.solidus && denier == other.denier);
    }

    explicit operator bool() const
    {
        return livre != 0 || solidus != 0 || denier != 0;
    };

    constexpr coin_number_t livre_number() const
    {
        return livre;
    };

    constexpr coin_number_t solidus_number() const
    {
        return solidus;
    };

    constexpr coin_number_t denier_number() const
    {
        return denier;
    };

  private:
    coin_number_t livre, solidus, denier;

    friend std::ostream &operator<<(std::ostream &os, const Moneybag &moneybag)
    {
        return os << "(" + std::to_string(moneybag.livre) +
                         (moneybag.livre == 1 ? " livr, " : " livres, ") +
                         std::to_string(moneybag.solidus) +
                         (moneybag.solidus == 1 ? " solidus, " : " soliduses, ") +
                         std::to_string(moneybag.denier) +
                         (moneybag.denier == 1 ? " denier)" : " deniers)");
    }
};

constexpr const Moneybag operator*(Moneybag::coin_number_t multiplier, const Moneybag &moneybag)
{
    return moneybag * multiplier;
}

class Value
{
  public:
    constexpr Value() : value(0)
    {
    }
    constexpr Value(const Moneybag::coin_number_t &value) : value(value)
    {
    }

    constexpr Value(const Moneybag &moneybag)
    {
        auto mul1 = overflow_mul(moneybag.livre_number(),
                                 static_cast<Moneybag::coin_number_t>(livre_to_denier));
        auto mul2 = overflow_mul(moneybag.solidus_number(),
                                 static_cast<Moneybag::coin_number_t>(solidus_to_denier));
        auto add = overflow_add(mul1, mul2);
        auto res = overflow_add(add, moneybag.denier_number());

        value = res;
    }

    constexpr Value(const Value &) = default;
    ~Value() = default;

    constexpr Value &operator=(const Value &) = default;

    auto operator<=>(const Value &) const = default;

    constexpr auto operator<=>(const Moneybag::coin_number_t &value) const
    {
        return this->value <=> value;
    }

    constexpr bool operator==(const Moneybag::coin_number_t &value) const
    {
        return this->value == value;
    }

    constexpr bool operator==(const Value &value) const
    {
        return this->value == value.value;
    }

    explicit operator std::string() const
    {
        return std::to_string(value);
    };

  private:
    static constexpr unsigned int solidus_to_denier = 12;
    static constexpr unsigned int livre_to_denier = 240;

    Moneybag::coin_number_t value;
};

constexpr Moneybag Livre = Moneybag(1, 0, 0);
constexpr Moneybag Solidus = Moneybag(0, 1, 0);
constexpr Moneybag Denier = Moneybag(0, 0, 1);

#endif /*- MONEYBAG_H -*/