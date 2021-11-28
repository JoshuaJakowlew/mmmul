#pragma once

#include <memory>
#include <ranges>

#include <concepts/Printable.hpp>
#include <utility.hpp>
#include <index_sequence.hpp>

template <typename T>
class Mat
{
public:
    using buffer_t = std::unique_ptr<T[]>;
    using multiindex_t = std::vector<std::size_t>;
    using index_t = typename multiindex_t::value_type;

#pragma region constructor
    Mat(std::size_t free, std::size_t scott, std::size_t caly, std::size_t n) :
        Mat(buffer_t{new T[pow(n, free + scott + caly)]}, free, scott, caly, n)
    {
    }

    template <std::convertible_to<buffer_t> B>
    Mat(B&& buffer, std::size_t free, std::size_t scott, std::size_t caly, std::size_t n) :
        _buffer{std::forward<B>(buffer)},
        _free{free},
        _scott{scott},
        _caly{caly},
        _n{n},
        _size{pow(n, (free + scott + caly))}
    {
    }
#pragma endregion constructor

    template <Printable U>
    friend std::ostream& operator<< (std::ostream& out, Mat<U> const& mat);

#pragma region subscription
    template <std::convertible_to<multiindex_t> I>
    [[nodiscard]] auto operator[](I&& indices) const -> T const &
    {
        return _buffer[toOffset(std::forward<I>(indices))];
    }

    template <std::convertible_to<multiindex_t> I>
    [[nodiscard]] auto operator[](I&& indices) -> T&
    {
        return const_cast<T&>(std::as_const(*this)[std::forward<I>(indices)]);
    }

    template <
        std::convertible_to<multiindex_t> I1 = multiindex_t,
        std::convertible_to<multiindex_t> I2 = multiindex_t,
        std::convertible_to<multiindex_t> I3 = multiindex_t
    >
    [[nodiscard]] auto operator()(I1&& free, I2&& scott, I3&& caly) const -> T const&
    {
        std::size_t offset = 0;

        offset += toOffset(std::forward<I1>(free), free.size() + scott.size() + caly.size(), _n);
        offset += toOffset(std::forward<I2>(scott), scott.size() + caly.size(), _n);
        offset += toOffset(std::forward<I3>(caly), caly.size(), _n);

        return _buffer[offset];
    }

    template <
        std::convertible_to<multiindex_t> I1 = multiindex_t,
        std::convertible_to<multiindex_t> I2 = multiindex_t,
        std::convertible_to<multiindex_t> I3 = multiindex_t
    >
    [[nodiscard]] auto operator()(I1&& free, I2&& scott, I3&& caly) -> T&
    {
        return const_cast<T&>(std::as_const(*this)(
            std::forward<I1>(free), std::forward<I2>(scott), std::forward<I3>(caly)
        ));
    }
#pragma endregion subscription

    [[nodiscard]] auto dissectScott() const -> std::vector<Mat<T>>;

private:
    buffer_t _buffer;
    std::size_t _free;
    std::size_t _scott;
    std::size_t _caly;
    std::size_t _n;
    std::size_t _size;

#pragma region offset
    template <std::convertible_to<multiindex_t> I = multiindex_t>
    std::size_t toOffset(I&& ids) const
    {
        return toOffset(std::forward<I>(ids), _free + _scott + _caly, _n);
    }

    template <std::convertible_to<multiindex_t> I = multiindex_t>
    std::size_t toOffset(I&& ids, std::size_t dims, std::size_t n) const
    {
        std::size_t offset = 0;
        for (std::size_t i = 0; i < ids.size(); ++i)
        {
            offset += ids[i] * pow(n, dims - i - 1);
        }

        return offset;
    }
#pragma endregion offset
};

template <Printable T>
std::ostream& operator<< (std::ostream& out, Mat<T> const& mat)
{
    out << "Mat[free=" << mat._free << ", scott=" << mat._scott << ", caly=" << mat._caly 
        << "], N=" << mat._n
        << "\n[";

    for (std::size_t i = 0; i < mat._size - 1; ++i)
    {
        out << mat._buffer[i] << ", ";
    }

    out << mat._buffer[mat._size - 1] << "]\n";

    return out;
}

template <typename T>
[[nodiscard]] auto Mat<T>::dissectScott() const -> std::vector<Mat<T>>
{
    auto sections = std::vector<Mat<T>>{};
    sections.reserve(pow(_n, _scott));

    auto free = index_sequence(_free, _n);
    auto scott = index_sequence(_scott, _n);
    auto caly = index_sequence(_caly, _n);
        
    for (auto&& s : scott)
    {
        sections.emplace_back(_free, 0, _caly, _n);

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
