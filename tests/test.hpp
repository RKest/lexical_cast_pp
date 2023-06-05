#pragma once

#include "../include/lpp/lexical_cast.hpp"

#include <variant>

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

struct Foo {
    int i;

    friend auto operator==(const Foo &, const Foo &) -> bool = default;
};

template<> auto lpp::of<Foo> = lpp::type_list<int>{};

struct Bar {
    double d;
    std::string s;

    friend auto operator==(const Bar &, const Bar &) -> bool = default;
};

template<> auto lpp::of<Bar> = lpp::type_list<double, std::string>{};

using FooBar = std::variant<Foo, Bar>;
template<> auto lpp::of<FooBar> = [](std::string_view name) {
    if (name == "Foo") { return lpp::extraction_info<FooBar>::of<Foo>(); }
    if (name == "Bar") { return lpp::extraction_info<FooBar>::of<Bar>(); }
    throw std::runtime_error("Invalid FooBar :c");
};

struct Big {
    Example ex;
    FooBar fb1;
    FooBar fb2;

    friend auto operator==(const Big &, const Big &) -> bool = default;
};

template<> auto lpp::of<Big> = lpp::type_list<Example, FooBar, FooBar>{};

