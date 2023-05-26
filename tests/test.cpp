#include "../include/lpp/lexical_cast.hpp"

#include "gmock/gmock.h"

using namespace lpp;
using namespace lpp::aux;

TEST(Split, Test) {
    auto a = split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 3UL, 2UL};
    EXPECT_EQ(a[0], "Lorem");
    EXPECT_EQ(a[1], "ipsum dolor sit");
    EXPECT_EQ(a[2], "amet consectetur");
}

TEST(Split, Test2) {
    auto a = split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 1UL, 1UL, 3UL};
    EXPECT_EQ(a[0], "Lorem");
    EXPECT_EQ(a[1], "ipsum");
    EXPECT_EQ(a[2], "dolor");
    EXPECT_EQ(a[3], "sit amet consectetur");
}

TEST(Split, Test3) {
    auto a = split{"Lorem ipsum dolor sit amet consectetur", ' ', 3UL, 1UL, 1UL, 1UL};
    EXPECT_EQ(a[0], "Lorem ipsum dolor");
    EXPECT_EQ(a[1], "sit");
    EXPECT_EQ(a[2], "amet");
    EXPECT_EQ(a[3], "consectetur");
}


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

TEST(ExtractoinTest, ShouldExtractCustomType) {
    std::string_view vec{"1 string 2 2.2"};
    auto type = lpp::cast<Example>(vec, ' ');
    auto expected = Example{1, "string", {2, 2.2}};

    EXPECT_EQ(type, expected);
}

// Dynamic Extraction

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

TEST(ExtractionTest, ShouldExtractDynamically) {
    constexpr std::string_view vec{"Foo 1"};
    auto foo_or_bar = lpp::cast<FooBar>(vec, ' ');

    EXPECT_EQ(std::get<Foo>(foo_or_bar), Foo{1});

    constexpr std::string_view vec2{"Bar 1.1 Some_string"};
    auto bar_or_foo = lpp::cast<FooBar>(vec2, ' ');

    ASSERT_TRUE(holds_alternative<Bar>(bar_or_foo));
    EXPECT_EQ(std::get<Bar>(bar_or_foo).d, 1.1);
    EXPECT_EQ(std::get<Bar>(bar_or_foo).s, "Some_string");
}

struct Big {
    Example ex;
    FooBar fb1;
    FooBar fb2;

    friend auto operator==(const Big &, const Big &) -> bool = default;
};

template<> auto lpp::of<Big> = lpp::type_list<Example, FooBar, FooBar>{};

TEST(ExtractionTest, ShouldDoBigExtraction) {
    constexpr std::string_view rep{"1 string 2 2.2 Foo 3 Bar 1.1 text"};

    auto type = lpp::cast<Big>(rep, ' ');
    auto expected = Big{
            .ex = Example{
                    .i = 1,
                    .s = "string",
                    .inner = Inner{.j = 2, .d = 2.2}},
            .fb1 = Foo{3},
            .fb2 = Bar{1.1, "text"},
    };

    EXPECT_EQ(type, expected);
}