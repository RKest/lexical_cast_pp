#include "test.hpp"

#include "gmock/gmock.h"

TEST(Split, Test) {
    auto a = lpp::aux::split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 3UL, 2UL};
    EXPECT_EQ(a[0], "Lorem");
    EXPECT_EQ(a[1], "ipsum dolor sit");
    EXPECT_EQ(a[2], "amet consectetur");
}
TEST(Split, Test2) {
    auto a = lpp::aux::split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 1UL, 1UL, 3UL};
    EXPECT_EQ(a[0], "Lorem");
    EXPECT_EQ(a[1], "ipsum");
    EXPECT_EQ(a[2], "dolor");
    EXPECT_EQ(a[3], "sit amet consectetur");
}
TEST(Split, Test3) {
    auto a = lpp::aux::split{"Lorem ipsum dolor sit amet consectetur", ' ', 3UL, 1UL, 1UL, 1UL};
    EXPECT_EQ(a[0], "Lorem ipsum dolor");
    EXPECT_EQ(a[1], "sit");
    EXPECT_EQ(a[2], "amet");
    EXPECT_EQ(a[3], "consectetur");
}

TEST(StaticExtractionTest, ShouldExtractCustomType) {
    EXPECT_EQ(lpp::cast<Example>("1 string 2 2.2", ' '), (Example{1, "string", {2, 2.2}}));
}
TEST(StaticExtractionTest, ShouldErrorWhenProvidedWithTooFewArguments) {
    EXPECT_THAT([] { [[maybe_unused]] auto _ = lpp::cast<Example>("1 string 2", ' '); },
                testing::ThrowsMessage<std::invalid_argument>("Too few arguments"));
}
TEST(StaticExtractionTest, ShouldErrorWhenProvidedWithNo) {
    EXPECT_THAT([] { [[maybe_unused]] auto _ = lpp::cast<Example>("", ' '); },
                testing::ThrowsMessage<std::invalid_argument>("Too few arguments"));
}

TEST(DynamicExtractionTest, ShouldExtract) {
    auto foo_or_bar = lpp::cast<FooBar>("Foo 1", ' ');
    EXPECT_EQ(std::get<Foo>(foo_or_bar), (Foo{1}));
}
TEST(DynamicExtractionTest, ShouldExtract2) {
    auto foo_or_bar = lpp::cast<FooBar>("Bar 1.1 Some_string", ' ');
    EXPECT_EQ(std::get<Bar>(foo_or_bar), (Bar{1.1, "Some_string"}));
}
TEST(DynamicExtractionTest, ShouldErrorWhenProvidedWithTooFewArguments) {
    EXPECT_THAT([] { [[maybe_unused]] auto _ = lpp::cast<FooBar>("Foo", ' '); },
                testing::ThrowsMessage<std::invalid_argument>("Too few arguments"));
}

TEST(CompoundExtractionTest, ShouldDoBigExtraction) {
    auto expected = Big{
            .ex = Example{
                    .i = 1,
                    .s = "string",
                    .inner = Inner{.j = 2, .d = 2.2}},
            .fb1 = Foo{3},
            .fb2 = Bar{1.1, "text"},
    };
    EXPECT_EQ(lpp::cast<Big>("1 string 2 2.2 Foo 3 Bar 1.1 text", ' '), expected);
}