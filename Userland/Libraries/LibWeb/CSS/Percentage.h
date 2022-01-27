/*
 * Copyright (c) 2022, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/Variant.h>
#include <LibWeb/CSS/Length.h>

namespace Web::CSS {

class Percentage {
public:
    explicit Percentage(int value)
        : m_value(value)
    {
    }

    explicit Percentage(float value)
        : m_value(value)
    {
    }

    float value() const { return m_value; }
    float as_fraction() const { return m_value * 0.01f; }

    String to_string() const
    {
        return String::formatted("{}%", m_value);
    }

    bool operator==(Percentage const& other) const { return m_value == other.m_value; }
    bool operator!=(Percentage const& other) const { return !(*this == other); }

private:
    float m_value;
};

template<typename T>
class PercentageOr {
public:
    PercentageOr(T t)
        : m_value(move(t))
    {
    }

    PercentageOr(Percentage percentage)
        : m_value(move(percentage))
    {
    }

    PercentageOr(NonnullRefPtr<CalculatedStyleValue> calculated)
        : m_value(move(calculated))
    {
    }

    virtual ~PercentageOr() { }

    PercentageOr<T>& operator=(T t)
    {
        m_value = move(t);
        return *this;
    }

    PercentageOr<T>& operator=(Percentage percentage)
    {
        m_value = move(percentage);
        return *this;
    }

    bool is_percentage() const { return m_value.template has<Percentage>(); }
    bool is_calculated() const { return m_value.template has<NonnullRefPtr<CalculatedStyleValue>>(); }

    Percentage const& percentage() const
    {
        VERIFY(is_percentage());
        return m_value.template get<Percentage>();
    }

    virtual T resolve_calculated(NonnullRefPtr<CalculatedStyleValue> const&, Layout::Node const&, [[maybe_unused]] T const& reference_value) const
    {
        VERIFY_NOT_REACHED();
    }

    T resolved(Layout::Node const& layout_node, T const& reference_value) const
    {
        return m_value.visit(
            [&](T const& t) {
                return t;
            },
            [&](Percentage const& percentage) {
                return reference_value.percentage_of(percentage);
            },
            [&](NonnullRefPtr<CalculatedStyleValue> const& calculated) {
                return resolve_calculated(calculated, layout_node, reference_value);
            });
    }

    String to_string() const
    {
        if (is_percentage())
            return m_value.template get<Percentage>().to_string();

        return m_value.template get<T>().to_string();
    }

    bool operator==(PercentageOr<T> const& other) const
    {
        if (is_calculated())
            return false;
        if (is_percentage() != other.is_percentage())
            return false;
        if (is_percentage())
            return (m_value.template get<Percentage>() == other.m_value.template get<Percentage>());
        return (m_value.template get<T>() == other.m_value.template get<T>());
    }
    bool operator!=(PercentageOr<T> const& other) const { return !(*this == other); }

protected:
    bool is_t() const { return m_value.template has<T>(); }
    T const& get_t() const { return m_value.template get<T>(); }

private:
    Variant<T, Percentage, NonnullRefPtr<CalculatedStyleValue>> m_value;
};

template<typename T>
bool operator==(PercentageOr<T> const& percentage_or, T const& t)
{
    return percentage_or == PercentageOr<T> { t };
}

template<typename T>
bool operator==(T const& t, PercentageOr<T> const& percentage_or)
{
    return t == percentage_or;
}

template<typename T>
bool operator==(PercentageOr<T> const& percentage_or, Percentage const& percentage)
{
    return percentage_or == PercentageOr<T> { percentage };
}

template<typename T>
bool operator==(Percentage const& percentage, PercentageOr<T> const& percentage_or)
{
    return percentage == percentage_or;
}

class LengthPercentage : public PercentageOr<Length> {
public:
    using PercentageOr<Length>::PercentageOr;

    bool is_length() const { return is_t(); }
    Length const& length() const { return get_t(); }
    virtual Length resolve_calculated(NonnullRefPtr<CalculatedStyleValue> const&, Layout::Node const&, Length const& reference_value) const override;
};

}

template<>
struct AK::Formatter<Web::CSS::Percentage> : Formatter<StringView> {
    ErrorOr<void> format(FormatBuilder& builder, Web::CSS::Percentage const& percentage)
    {
        return Formatter<StringView>::format(builder, percentage.to_string());
    }
};

template<>
struct AK::Formatter<Web::CSS::LengthPercentage> : Formatter<StringView> {
    ErrorOr<void> format(FormatBuilder& builder, Web::CSS::LengthPercentage const& length_percentage)
    {
        return Formatter<StringView>::format(builder, length_percentage.to_string());
    }
};
