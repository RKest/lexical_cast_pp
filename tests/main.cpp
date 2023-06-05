#include <chrono>
#include <iostream>
#include "../include/lpp/lexical_cast.hpp"
#include <boost/algorithm/string.hpp>

struct Inner {
    int j;
    double d;

    friend auto operator==(const Inner &, const Inner &) -> bool = default;
};

template<> auto lpp::of<Inner> = lpp::type_list<int, double>{};

struct Example {
    int i;
    std::string s;
    Inner inner;

    friend auto operator==(const Example &, const Example &) -> bool = default;
};

template<> auto lpp::of<Example> = lpp::type_list<int, std::string, Inner>{};

int main()
{
    std::string_view vec{"1 string 2 2.2"};
    auto now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; ++i)
    {
        auto type = lpp::cast<Example>(vec, ' ');
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "Solution took:\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - now).count() << '\n';

    auto now2 = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; ++i) {
        std::vector<std::string> s;
        boost::split(s, vec, boost::is_any_of(" "));
        auto ex = Example{
                boost::lexical_cast<int>(s[0]),
                s[1],
                {
                        boost::lexical_cast<int>(s[2]),
                        boost::lexical_cast<double>(s[3]),
                },
        };
    }
    auto end2 = std::chrono::system_clock::now();
    std::cout << "Naive took:\t" << std::chrono::duration_cast<std::chrono::microseconds>(end2 - now2).count() << '\n';
}