#include <iostream>
#include <random>
#include <ranges>
#include <Matrix.hpp>

// FIXME: Works only with numbers
template <typename T>
Matrix<T> iotaMat(std::size_t a, std::size_t b, std::size_t c, std::size_t n)
{
    auto buffer_size = pow(n, a + b + c);
    auto buffer = typename Matrix<T>::buffer_t{new T[buffer_size]};

    for (std::size_t i = 0; i < buffer_size; ++i)
    {
        buffer[i] = static_cast<T>(i);
    }  

    return Matrix<T>{std::move(buffer), a, b, c, n};
}

int main()
{
    constexpr std::size_t a = 1;
    constexpr std::size_t b = 1;
    constexpr std::size_t c = 1;
    constexpr std::size_t n = 3;

    auto mat = iotaMat<float>(a, b, c, n);
    // //auto mat = Matrix<float>(a, b, c, n);
    std::cout << mat << std::endl;

    // auto perms = indicies(a, n);
    // //std::cout << perms << std::endl;

    // auto x = mat[{1, 2, 2}];
    // std::cout << x << std::endl;

    // auto y = mat({1}, {2}, {2});
    // std::cout << y << std::endl;

    auto sections = mat.dissectScott();
    for (auto&& section : sections)
    {
        std::cout << section << std::endl;
    }

    // std::cout << 
}