#pragma once

#include <Mat.hpp>

template <typename T>
Mat<T> zeroMat(std::size_t a, std::size_t b, std::size_t c, std::size_t n)
{
    auto buffer_size = pow(n, a + b + c);
    auto buffer = typename Mat<T>::buffer_t{new T[buffer_size]};

    for (std::size_t i = 0; i < buffer_size; ++i)
    {
        buffer[i] = static_cast<T>(i);
    }  

    return Mat<T>{std::move(buffer), a, b, c, n};
}

template <typename T>
Mat<T> iotaMat(std::size_t a, std::size_t b, std::size_t c, std::size_t n)
{
    auto buffer_size = pow(n, a + b + c);
    auto buffer = typename Mat<T>::buffer_t{new T[buffer_size]};

    for (std::size_t i = 0; i < buffer_size; ++i)
    {
        buffer[i] = static_cast<T>(i);
    }  

    return Mat<T>{std::move(buffer), a, b, c, n};
}

template <typename T>
auto zeroMuConvolutionProduct(Mat<T> const & A, Mat<T> const & B) -> Mat<T>
{
    auto caly = index_sequence(A._c, A._n);
    auto freeL = index_sequence(A._a, A._n);
    auto freeM = index_sequence(B._c, B._n);
    
    auto C = zeroMat<T>(A._a, 0, B._c, A._n);

    for (auto&& c : caly)
    {
        for (auto&& l : freeL)
        {
            for (auto&& m : freeM)
            {
                auto elem = A(l, {}, c) * B({}, c, m);
                C(l, {}, m) = elem;
            }
        }
    }

    return C;
}

template<typename T>
auto cannonLambdaMuConvolutionProduct(Mat<T> const & A, Mat<T> const & B) -> Mat<T>
{
    auto As = A.dissectB();
    auto Bs = B.dissectA();
    std::vector<Mat<T>> Cs{As.size(), zeroMat()};
    
    /*
    for s
    for l
    for m
    {
        Clsm = sigma c
        {
            Alsc * Bscm
        }
    }
    */
}
