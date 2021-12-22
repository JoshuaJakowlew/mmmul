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
        _a{free},
        _b{scott},
        _c{caly},
        _n{n},
        _size{pow(n, (free + scott + caly))}
    {
    }

    Mat(std::vector<Mat<T>> const & scottSections, int group)
    {
        if (group == 0) fromSectionsA(scottSections);
        else if (group == 1) fromSectionsB(scottSections);
        else fromSectionsC(scottSections);
    }

    void fromSectionsA(std::vector<Mat<T>> const & scottSections)
    {
        _n = scottSections[0]._n;
        _a = static_cast<std::size_t>(
            std::round(std::log(scottSections.size()) / std::log(_n))
        );
        _b = scottSections[0]._b;
        _c = scottSections[0]._c;

        _size = pow(_n, (_a + _b + _c));
        _buffer = buffer_t{new T[_size]};

        auto as = index_sequence(_a, _n);
        auto bs = index_sequence(_b, _n);
        auto cs = index_sequence(_c, _n);

        for (auto&& a : as)
        {
            const auto sectionIndex = toOffset(a, _a, _n);

            for (auto&& b : bs)
            {
                for (auto&& c : cs)
                {
                    auto&& elem = scottSections[sectionIndex]({}, std::move(b), std::move(c));
                    (*this)(std::move(a), std::move(b), std::move(c)) = std::move(elem);
                }
            }
        }
    }

    void fromSectionsB(std::vector<Mat<T>> const & scottSections)
    {
        _n = scottSections[0]._n;
        _a = scottSections[0]._a;
        _b = static_cast<std::size_t>(
            std::round(std::log(scottSections.size()) / std::log(_n))
        );
        _c = scottSections[0]._c;

        _size = pow(_n, (_a + _b + _c));
        _buffer = buffer_t{new T[_size]};

        auto as = index_sequence(_a, _n);
        auto bs = index_sequence(_b, _n);
        auto cs = index_sequence(_c, _n);

        for (auto&& b : bs)
        {
            const auto sectionIndex = toOffset(b, _b, _n);

            for (auto&& a : as)
            {
                for (auto&& c : cs)
                {
                    auto&& elem = scottSections[sectionIndex](std::move(a), {}, std::move(c));
                    (*this)(std::move(a), std::move(b), std::move(c)) = std::move(elem);
                }
            }
        }
    }

    void fromSectionsC(std::vector<Mat<T>> const & scottSections)
    {
        _n = scottSections[0]._n;
        _a = scottSections[0]._a;
        _b = scottSections[0]._b;
        _c = static_cast<std::size_t>(
            std::round(std::log(scottSections.size()) / std::log(_n))
        );

        _size = pow(_n, (_a + _b + _c));
        _buffer = buffer_t{new T[_size]};

        auto as = index_sequence(_a, _n);
        auto bs = index_sequence(_b, _n);
        auto cs = index_sequence(_c, _n);

        for (auto&& c : cs)
        {
            const auto sectionIndex = toOffset(c, _c, _n);

            for (auto&& a : as)
            {
                for (auto&& b : bs)
                {
                    auto&& elem = scottSections[sectionIndex](std::move(a), std::move(c), {});
                    (*this)(std::move(a), std::move(b), std::move(c)) = std::move(elem);
                }
            }
        }
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

    [[nodiscard]] auto dissectA() const -> std::vector<Mat<T>>;
    [[nodiscard]] auto dissectB() const -> std::vector<Mat<T>>;
    [[nodiscard]] auto dissectC() const -> std::vector<Mat<T>>;


    buffer_t _buffer;
    std::size_t _a;
    std::size_t _b;
    std::size_t _c;
    std::size_t _n;
    std::size_t _size;

private:

#pragma region offset
    template <std::convertible_to<multiindex_t> I = multiindex_t>
    std::size_t toOffset(I&& ids) const
    {
        return toOffset(std::forward<I>(ids), _a + _b + _c, _n);
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
    out << "\nMat[a=" << mat._a << ", b=" << mat._b << ", c=" << mat._c 
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
[[nodiscard]] auto Mat<T>::dissectA() const -> std::vector<Mat<T>>
{
    auto sections = std::vector<Mat<T>>{};
    sections.reserve(pow(_n, _a));

    auto as = index_sequence(_a, _n);
    auto bs = index_sequence(_b, _n);
    auto cs = index_sequence(_c, _n);
        
    for (auto&& a : as)
    {
        sections.emplace_back(0, _b, _c, _n);

        for (auto&& b : bs)
        {
            for (auto&& c : cs)
            {
                auto&& elem = (*this)(std::move(a), std::move(b), std::move(c));
                sections.back()({}, std::move(b), std::move(c)) = std::move(elem);
            }
        }
    }

    return sections;
}

template <typename T>
[[nodiscard]] auto Mat<T>::dissectB() const -> std::vector<Mat<T>>
{
    auto sections = std::vector<Mat<T>>{};
    sections.reserve(pow(_n, _b));

    auto as = index_sequence(_a, _n);
    auto bs = index_sequence(_b, _n);
    auto cs = index_sequence(_c, _n);
        
    for (auto&& b : bs)
    {
        sections.emplace_back(_a, 0, _c, _n);

        for (auto&& a : as)
        {
            for (auto&& c : cs)
            {
                auto&& elem = (*this)(std::move(a), std::move(b), std::move(c));
                sections.back()(std::move(a), {}, std::move(c)) = std::move(elem);
            }
        }
    }

    return sections;
}

template <typename T>
[[nodiscard]] auto Mat<T>::dissectC() const -> std::vector<Mat<T>>
{
    auto sections = std::vector<Mat<T>>{};
    sections.reserve(pow(_n, _c));

    auto as = index_sequence(_a, _n);
    auto bs = index_sequence(_b, _n);
    auto cs = index_sequence(_c, _n);
        
    for (auto&& c : cs)
    {
        sections.emplace_back(_a, _b, 0, _n);

        for (auto&& a : as)
        {
            for (auto&& b : bs)
            {
                auto&& elem = (*this)(std::move(a), std::move(b), std::move(c));
                sections.back()(std::move(a), std::move(b), {}) = std::move(elem);
            }
        }
    }

    return sections;
}
