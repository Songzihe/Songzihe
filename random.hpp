#ifndef random_hpp
#define random_hpp

#include <iostream>

namespace ra::random {


    class linear_congruential_generator {
        typedef unsigned long long int_type; //(a)

    private:
        int_type a;
        int_type c;
        int_type m;
        int_type s;
        int_type xi;
    public:
        static int_type default_seed(); //(i)
        linear_congruential_generator(int_type a, int_type c, int_type m, int_type s = default_seed()); //(b)
        linear_congruential_generator(const linear_congruential_generator &&other); //(c)
        linear_congruential_generator &operator=(const linear_congruential_generator &&other); //(c)

        linear_congruential_generator(const linear_congruential_generator &other); //(d)
        linear_congruential_generator &operator=(const linear_congruential_generator &other); //(d)

        ~linear_congruential_generator(); //(e)
        int_type multiplier() const; //(f)
        int_type increment() const; //(g)
        int_type modulus() const; //(h)
        int_type state() const; //(h)

        void seed(int_type s); //(j)
        int_type operator()(); //(k)
        void discard(unsigned long long n); //(l)
        int_type min() const; //(m)
        int_type max() const; //(n)
        bool operator==(const linear_congruential_generator &other) const; //(o)
        bool operator!=(const linear_congruential_generator &other) const; //(o)
    };

    std::ostream& operator<<(std::ostream &out, const linear_congruential_generator &lcg);
}

#endif