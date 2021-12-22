#include <iostream>
#include <random>
#include <ranges>
#include <Mat.hpp>
#include <operations.hpp>

int main()
{
    constexpr std::size_t a = 1;
    constexpr std::size_t b = 1;
    constexpr std::size_t c = 1;
    constexpr std::size_t n = 3;

    auto A = iotaMat<float>(a, b, c, n);
    auto B = iotaMat<float>(a, b, c, n);

    std::cout << "Matrix A" << std::endl;
    std::cout << A << std::endl;

    std::cout << "Matrix B" << std::endl;
    std::cout << B << std::endl;

    auto As = A.dissectB();
    auto Bs = B.dissectA();
    
    std::cout << "Sections of A" << std::endl;
    std::cout << As << std::endl;

    std::cout << "Sections of B" << std::endl;
    std::cout << Bs << std::endl;

    std::vector<Mat<float>> Cs;
    for (std::size_t i = 0; i < As.size(); ++i)
    {
        std::cout << "a" << i << " * b" << i << std::endl;
        auto cs = zeroMuConvolutionProduct(As[i], Bs[i]);
        std::cout << cs << std::endl;
        Cs.emplace_back(std::move(cs));
    }

    std::cout << "C from sections" << std::endl;
    auto C = Mat<float>(Cs, 1);
    std::cout << C << std::endl;

}