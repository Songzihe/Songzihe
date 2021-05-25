#include <iostream>
#include "ra/random.hpp"

using namespace ra::random;

int main() {
    linear_congruential_generator lcg1 = linear_congruential_generator(2, 0, 9);
    std::cout << "linear_congruential_generator_v1:" << std::endl;
    std::cout << lcg1.state() << " ";
    for (int i = 0; i < 30; ++i) {
        std::cout << lcg1() << " ";
    }

    std::cout << std::endl;

    linear_congruential_generator lcg2 = linear_congruential_generator(4, 1, 9, 0);
    std::cout << "linear_congruential_generator_v2:" << std::endl;
    std::cout << lcg2.state() << " ";
    for (int i = 0; i < 30; ++i) {
        std::cout << lcg2() << " ";
    }
    std::cout << std::endl;

    std::cout << (lcg1 == lcg2) << std::endl;
    std::cout << (lcg1 != lcg2) << std::endl;
    std::cout << (lcg1 == lcg1) << std::endl;

    return 0;
}