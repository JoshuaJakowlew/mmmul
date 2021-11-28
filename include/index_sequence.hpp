#pragma once

#include <vector>
#include <ranges>

#include <utility.hpp>

namespace detail
{
    struct index_sequence_state
    {
        index_sequence_state(std::size_t digits, std::size_t radix) :
            digits(digits),
            radix{radix}
        {}

        std::vector<std::size_t> digits;
        std::size_t radix;

        void next()
        {
            std::size_t carry = 1;
            for (auto it = digits.rbegin(); it != digits.rend(); ++it)
            {
                if (0 == carry)
                    break;

                *it += carry;
                carry = *it / radix;
                *it = *it % radix;
            }
        }
    };
}

[[nodiscard]] auto index_sequence(std::size_t digits, std::size_t radix)
{
    std::size_t size = pow(radix, digits);

    auto increment = [
        state = detail::index_sequence_state {digits, radix}
    ] (auto i) mutable
    {
        [[unlikely]] if (0 == i)
            return state.digits;
            
        state.next();
        return state.digits;
    };

    return std::views::iota(0uz, size) | std::views::transform(increment);
}
