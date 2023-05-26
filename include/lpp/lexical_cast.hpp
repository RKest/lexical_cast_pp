//                 Copyright Max Ind 2023
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//              Contact: dmmaster335@gmail.com

#pragma once

#if __cplusplus < 202002L
#error "Lexical Cast++ requires C++20 support"
#endif

#include <iostream>

#if __has_include(<boost/lexical_cast.hpp>)
#include <boost/lexical_cast.hpp>
#else
namespace boost
{
    template<typename Target, typename Source>
    Target lexical_cast(const Source &);
}
#endif

#define LEXICAL_CAST_PLUS_PLUS_NAMESPACE_BEGIN \
namespace lpp {\
inline namespace v0_1 {

#define LEXICAL_CAST_PLUS_PLUS_NAMESPACE_END \
}                                            \
}

LEXICAL_CAST_PLUS_PLUS_NAMESPACE_BEGIN
template<typename ...Ts> struct type_list {};
namespace aux
{
    template<typename TypeList> struct size {};
    template<typename ...Ts> struct size<type_list<Ts...>> { constexpr static auto value = sizeof...(Ts); };
    template<typename TypeList> constexpr auto size_v = size<TypeList>::value;
    template<typename Signature> struct args;
    template<typename R, typename ...Args> struct args<R (*)(Args...)> { using type = type_list<Args...>; };
    template<typename Signature> using args_t = typename args<Signature>::type;
    template<typename Signature> using args_t_not_ptr = args_t<decltype(+std::declval<Signature>())>;
    template<typename Signature> struct a_return;
    template<typename R, typename ...Args> struct a_return<R (*)(Args...)> { using type = R; };
    template<typename Signature> using return_t = typename a_return<Signature>::type;
    template<typename Signature> using return_t_not_ptr = return_t<decltype(+std::declval<Signature>())>;
    template<typename T, std::size_t I> struct enumerated {};
    template<typename TypeList> struct enumerate;
    template<typename ...Ts> struct enumerate<type_list<Ts...>> {
        using type = decltype([]<std::size_t ...Is>(std::index_sequence<Is...>){
            return type_list<enumerated<Ts, Is>...>{};
        }(std::make_index_sequence<sizeof...(Ts)>{}));
    };
    template<typename TypeList> using enumerate_t = typename enumerate<TypeList>::type;
    template<std::size_t N, typename ...Ts> struct nth_pack_impl;
    template<typename T0, typename... T> struct nth_pack_impl<0, T0, T...> { using type = T0; };
    template<std::size_t N, typename T0, typename... T> struct nth_pack_impl<N, T0, T...> { using type = typename nth_pack_impl<N - 1, T...>::type; };
    template<std::size_t N, typename ...Ts> using nth_pack = typename nth_pack_impl<N, Ts...>::type;
    template<std::size_t N, typename TypeList> struct nth_impl;
    template<std::size_t N, typename ...Ts> struct nth_impl<N, type_list<Ts...>> { using type = typename nth_pack_impl<N, Ts...>::type; };
    template<std::size_t N, typename TypeList> using nth = typename nth_impl<N, TypeList>::type;

    static_assert(size_v<type_list<int, char>> == 2);
    static_assert(std::same_as<args_t<void(*)(int, char)>, type_list<int, char>>);
    static_assert(std::same_as<return_t<void(*)(int, char)>, void>);
    static_assert(std::same_as<enumerate_t<type_list<int, char>>, type_list<enumerated<int, 0>, enumerated<char, 1>>>);
    static_assert(std::same_as<nth_pack<1, int, char>, char>);
    static_assert(std::same_as<nth<1, type_list<int, char>>, char>);

    template <auto Start, auto End, auto Inc, class F>
    constexpr void constexpr_for(F&& f)
    {
        if constexpr (Start < End)
        {
            f(std::integral_constant<decltype(Start), Start>());
            constexpr_for<Start + Inc, End, Inc>(f);
        }
    }

    template <bool cond_v, typename Then, typename OrElse>
    constexpr auto constexpr_if(Then&& then, OrElse&& or_else) -> decltype(auto) {
        if constexpr (cond_v) { return std::forward<Then>(then);}
        else { return std::forward<OrElse>(or_else); }
    }

    // The behaviour for pivot equal to the number of words in the string_view is undefined
    constexpr static auto half_split(std::string_view whole, char delimiter, std::size_t pivot) -> std::array<std::string_view, 2> {
        if (pivot == 0) { return {"", whole}; }
        std::array<std::string_view, 2> ret;
        std::size_t sz = 0;
        auto it = begin(whole);
        while (pivot-- > 0) {
            auto temp = std::find(it, end(whole), delimiter) + 1;
            sz += static_cast<size_t>(std::distance(it, temp));
            it = temp;
        }
        auto pivot_iter = begin(whole) + sz;
        ret[0] = {begin(whole), pivot_iter - 1};
        ret[1] = {pivot_iter, end(whole)};
        return ret;
    }

    static_assert(half_split("Lorem ipsum dolor sit amet consectetur", ' ', 1UL)[0] == "Lorem");
    static_assert(half_split("Lorem ipsum dolor sit amet consectetur", ' ', 4UL)[1] == "amet consectetur");
    static_assert(half_split("Lorem ipsum dolor sit amet consectetur", ' ', 0UL)[0] == "");

    template<std::size_t S>
    class split {
        using parts_t = std::array<std::string_view, S>;
        parts_t parts;

        constexpr static auto split_impl(std::string_view whole, char delimiter, auto... lens) -> parts_t {
            parts_t parts_init;
            std::size_t i = 0;
            std::ptrdiff_t cursor = 0;
            auto it = whole.begin();
            for (auto len: std::array<std::size_t, S>{ lens... }) {
                std::ptrdiff_t sz = 0;
                while (len-- > 0) {
                    auto temp = std::find(it, whole.end(), delimiter) + 1;
                    sz += std::distance(it, temp);
                    it = temp;
                }
                parts_init[i++] = std::string_view{whole.begin() + cursor, whole.begin() + cursor + sz - 1};
                cursor += sz;
            }
            return parts_init;
        }

    public:
        constexpr split(std::string_view view, char delimiter, std::same_as<std::size_t> auto... lens)
             : parts{split_impl(view, delimiter, lens...)} {}
        [[nodiscard]] constexpr auto operator[](std::size_t i) const { return parts[i]; }
    };

    template<typename ...Sz> split(std::string_view, char, Sz...) -> split<sizeof...(Sz)>;

    static_assert(split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 3UL, 2UL}[0] == "Lorem");
    static_assert(split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 3UL, 2UL}[1] == "ipsum dolor sit");
    static_assert(split{"Lorem ipsum dolor sit amet consectetur", ' ', 1UL, 3UL, 2UL}[2] == "amet consectetur");
}

template<typename T> constexpr auto of = [] {};
template<typename T> struct extraction_info;

namespace aux
{
    template<typename T> concept TypeListable = requires {[]<typename ...Ts>(type_list<Ts...>) {}(T{}); };
    template<typename T> concept IStreamable = requires(T t, std::istream &is) { is >> t; };
    template<typename T> concept StringLike = std::constructible_from<T, std::string_view> && !IStreamable<T>;
    template<typename T> concept Extractable = TypeListable<decltype(of<T>)>;
    template<typename T> concept DynamicallyExtractable = std::same_as<return_t_not_ptr<decltype(of<T>)>, extraction_info<T>>;

    template<IStreamable Target> constexpr auto extract_argc(std::string_view, char) -> std::size_t;
    template<StringLike Target> constexpr auto extract_argc(std::string_view, char) -> std::size_t;
    template<Extractable Target> constexpr auto extract_argc(std::string_view, char) -> std::size_t;
    template<DynamicallyExtractable Target> constexpr auto extract_argc(std::string_view, char) -> std::size_t;

    template<IStreamable Target> constexpr auto extract(std::string_view, char delimiter) -> Target;
    template<StringLike Target> constexpr auto extract(std::string_view, char) -> Target;
    template<Extractable Target> constexpr auto extract(std::string_view, char delimiter) -> Target;
    template<DynamicallyExtractable Target> constexpr auto extract(std::string_view, char delimiter) -> Target;
}

template<typename Common> struct extraction_info {
    std::size_t (*argc_functor)(std::string_view, char);
    Common (*factory_functor)(std::string_view, char);

    template<typename Concrete> constexpr static auto of() -> extraction_info<Common>
        requires (std::convertible_to<Concrete*, Common*> || std::convertible_to<Concrete, Common>) {
        return extraction_info<Common>{
            .argc_functor = aux::extract_argc<Concrete>,
            .factory_functor = [](std::string_view sv, char delim) -> Common { return aux::extract<Concrete>(sv, delim); }
        };
    }
};

namespace aux {
    template<StringLike Target> constexpr auto extract_argc(std::string_view, char) -> std::size_t { return 1UL; }
    template<StringLike Target> constexpr auto extract(std::string_view sv, char) -> Target { return sv; }

    template<IStreamable Target> constexpr auto extract_argc(std::string_view, char) -> std::size_t { return 1UL; }
    template<IStreamable Target> constexpr auto extract(std::string_view sv, char) -> Target { return boost::lexical_cast<Target>(sv); }

    template<Extractable Target> constexpr auto extract_argc(std::string_view sv, char delim) -> std::size_t {
        return [sv, delim]<typename... Ts>(type_list<Ts...>) { return (... + extract_argc<Ts>(sv, delim)); }(of<Target>);
    }

    struct argc_info { std::size_t skip; std::size_t take; };
    template<typename ...Ts> constexpr auto all_argc(std::string_view sv, char delimiter) -> std::array<argc_info, sizeof...(Ts)>
    {
        std::array<argc_info, sizeof...(Ts)> all_argc;
        std::size_t previous = 0;
        constexpr_for<0, sizeof...(Ts), 1>([&](auto i){
            const auto part = half_split(sv, delimiter, previous)[1];
            all_argc[i] = argc_info{previous, static_cast<std::size_t>(extract_argc<nth_pack<i, Ts...>>(part, delimiter))};
            previous += all_argc[i].take;
        });
        return all_argc;
    }
    template<Extractable Target> constexpr auto extract(std::string_view sv, char delimiter) -> Target {
        return [sv, delimiter]<typename ...Ts, std::size_t ...Is>(type_list<enumerated<Ts, Is>...>){
            const auto argc = all_argc<Ts...>(sv, delimiter);
            const auto split_sv = split{sv, delimiter, extract_argc<Ts>(split{sv, delimiter, argc[Is].skip, argc[Is].take}[1], delimiter)...};
            return Target{extract<Ts>(split_sv[Is], delimiter)...};
        }(enumerate_t<decltype(of<Target>)>{});
    }

    template<DynamicallyExtractable Target> constexpr auto extract_argc(std::string_view sv, char delimiter) -> std::size_t {
        return [sv, delimiter]<typename ...Ts, std::size_t ...Is>(type_list<enumerated<Ts, Is>...>) {
            const auto argc = all_argc<Ts...>(sv, delimiter);
            const auto info = std::invoke(of<Target>, extract<Ts>(split{sv, delimiter, argc[Is].skip, argc[Is].take}[1], delimiter)...);
            return (... + extract_argc<Ts>(sv, delimiter)) + info.argc_functor(sv, delimiter);
        }(enumerate_t<args_t_not_ptr<decltype(of<Target>)>>{});
    }
    template<DynamicallyExtractable Target> constexpr auto extract(std::string_view sv, char delimiter) -> Target {
        return [sv, delimiter]<typename ...Ts, std::size_t ...Is>(type_list<enumerated<Ts, Is>...>) {
            const auto split_sv = split{sv, delimiter, extract_argc<Ts>(sv, delimiter)...};
            const auto info = std::invoke(of<Target>, extract<Ts>(split_sv[Is], delimiter)...);
            const auto skip = (... + extract_argc<Ts>(sv, delimiter));
            const auto argc = info.argc_functor(sv, delimiter);
            return std::invoke(info.factory_functor, split(sv, delimiter, skip, argc)[1], delimiter);
        }(enumerate_t<args_t_not_ptr<decltype(of<Target>)>>{});
    }
}

template<typename Target> constexpr auto cast(std::string_view source, char delimiter) {
    return aux::extract<Target>(source, delimiter);
}

LEXICAL_CAST_PLUS_PLUS_NAMESPACE_END