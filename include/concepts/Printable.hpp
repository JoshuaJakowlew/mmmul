#pragma once

template <typename T>
concept Printable = requires(std::ostream& out, T const& x)
{
    { out << x } -> std::convertible_to<std::ostream&>;
};

template <Printable T>
std::ostream& operator<< (std::ostream& out, std::vector<T> const& vec)
{
    if (vec.empty())
    {
        out << "[]";
        return out;
    }

    out << '[';
    for (std::size_t i = 0; i < vec.size() - 1; ++i)
    {
        out << vec[i] << ", ";
    }
    out << vec.back() << ']';

    return out;
}
