#pragma once

#include <memory>
#include <cmath>
#include <ranges>

std::size_t pow(std::size_t x, std::size_t y)
{
    return static_cast<std::size_t>(std::pow(x, y));
}

template <typename T>
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

void inc_index(std::vector<std::size_t>& digits, std::size_t n)
{
    std::size_t carry = 1;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it)
    {
        if (0 == carry)
            break;

        *it += carry;
        carry = *it / n;
        *it = *it % n;
    }
}

// TODO: Make me O(ids) memory instead of O(n^ids * n)
std::vector<std::vector<std::size_t>> indicies(std::size_t ids, std::size_t n)
{
    std::vector<std::vector<std::size_t>> permutations;

    std::vector<std::size_t> digits(ids);
    permutations.emplace_back(digits.begin(), digits.end());

    std::size_t bound = pow(n, ids);

    for (std::size_t i = 1; i < bound; ++i)
    {
        inc_index(digits, n);
        permutations.emplace_back(digits.begin(), digits.end());
    }

    return permutations;
}

template <typename T>
class Matrix
{
public:
    using buffer_t = std::unique_ptr<T[]>;
    using multiindex_t = std::vector<std::size_t>;
    using index_t = typename multiindex_t::value_type;

    Matrix(std::size_t a, std::size_t b, std::size_t c, std::size_t n) :
        Matrix(buffer_t{new T[pow(n, a + b + c)]}, a, b, c, n)
    {
    }

    Matrix(buffer_t&& buffer, std::size_t a, std::size_t b, std::size_t c, std::size_t n) :
        _buffer{std::move(buffer)},
        _a{a},
        _b{b},
        _c{c},
        _n{n},
        _size{pow(n, (a + b + c))}
    {
    }

    template <typename U>
    friend std::ostream& operator<< (std::ostream& out, Matrix<U> const& mat);


    std::vector<Matrix<T>> dissectScott()
    {
        std::vector<Matrix<T>> sections;
        sections.reserve(_b);

        const auto free = indicies(_a, _n);
        const auto scott = indicies(_b, _n);
        const auto caly = indicies(_c, _n);
        
        for (auto&& s : scott)
        {
            sections.emplace_back(_a, 0, _c, _n);

            for (auto&& f : free)
            {
                for (auto&& c : caly)
                {
                    auto&& elem = (*this)(std::move(f), std::move(s), std::move(c));
                    sections.back()(std::move(f), {}, std::move(c)) = std::move(elem);
                }
            }
        }

        return sections;
    }

    T& operator[](multiindex_t const& ids)
    {
        return _buffer[toOffset(ids)];
    }

    T& operator()(multiindex_t const& a, multiindex_t const& b, multiindex_t const& c)
    {
        std::size_t offset = 0;

        offset += toOffset(a, a.size() + b.size() + c.size(), _n);
        offset += toOffset(b, b.size() + c.size()           , _n);
        offset += toOffset(c, c.size()                      , _n);

        return _buffer[offset];
    }

    std::size_t toOffset(multiindex_t const& ids)
    {
        return toOffset(std::move(ids), _a + _b + _c, _n);
    }

    std::size_t toOffset(multiindex_t const& ids, std::size_t dims, std::size_t n)
    {
        std::size_t offset = 0;
        for (std::size_t i = 0; i < ids.size(); ++i)
        {
            offset += ids[i] * pow(n, dims - i - 1);
        }

        return offset;
    }

private:
    buffer_t _buffer;
    std::size_t _;
    std::size_t _a;
    std::size_t _b;
    std::size_t _c;
    std::size_t _n;
    std::size_t _size;
};

template <typename T>
std::ostream& operator<< (std::ostream& out, Matrix<T> const& mat)
{
    out << "Mat[free=" << mat._a << ", scott=" << mat._b << ", caly=" << mat._c 
        << "], N=" << mat._n
        << "\n[";

    for (std::size_t i = 0; i < mat._size - 1; ++i)
    {
        out << mat._buffer[i] << ", ";
    }

    out << mat._buffer[mat._size - 1] << "]\n";

    return out;
}

