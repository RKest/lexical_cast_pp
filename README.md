# LexicalCast++
### Dependency and dynamic allocations free, `boost::lexical_cast` extension point
#####... dependency free only if you bring your own `boost::lexical_cast` ;)

### Motivational examples
>#### No forcing default constructors on your classes, or imperative directives
```c++
---------------------------------------------------------------------------------------
#include <sstream>                          | #include <lpp/lexical_cast.hpp>
                                            | 
struct S {                                  | struct S { 
    int i;                                  |   int i;
    std::string s;                          |   std::string s;
};                                          | };
                                            | 
istream& operator>>(istream& is, S& s)      | template<>
{                                           | auto lpp::of<S> = lpp::type_list<int, std::string>{};
    is >> s.i >> s.s;                       |
    return is;                              |
}                                           |
                                            |
int main()                                  | int main()
{                                           | {
    std::istringstream iss{"1 some_text"}   |   S s = lpp::cast<S>("1 some_text", ' ');
    S s; /* Default constructor necessary */| }
    iss >> s;                               |
}
```
>#### No more repetition
```c++
------------------------------------------------------------------------------------
#include <boost/split.hpp>                  | #include <lpp/lexical_cast.hpp>
#include <boost/lexical_cast.hpp>           | 
                                            | 
struct S { int i; double d; };              | struct S { int i; double d; };
struct T { S s1; S s2; };                   | struct T { S s1; S s2; };
                                            | 
S make_s(std::string s1, std::string s2)    | template<> 
{                                           | auto lpp::of<S> = lpp::type_list<int, double>;
    return {                                | 
        boost::lexical_cast<int>(s1),       | 
        boost::lexical_cast<double>(s2),    | 
    };                                      | 
}                                           | 
                                            | 
T make_t(std::string s, char delim)         | template<>
{                                           | auto lpp::of<T> = lpp::type_list<S, S>;
    std::vector<std::string> split_s;       | 
    boost::split(split_s, s,                | 
                 boost::is_any_of(delim));  | 
    return {                                | 
        make_s(split_s[0], split_s[1]),     | 
        make_s(split_s[0], split_s[1]),     | 
    };                                      | 
}                                           | 
                                            | 
int main()                                  | int main()
{                                           | {
    T t = make_t("20 2.5 60 7.5", ' ');     |   T t = lpp::cast<T>("20 2.5 60 7.5", ' ');
}                                           | }
```
> #### Declarative polymorphism

```c++
#include <lpp/lexical_cast.hpp>
#include <variant>
#include <cassert>

struct Circle { 
    double r;
};

struct Rectangle {
    double x;
    double y;
};

using Shape = std::variant<Circle, Shape>;

template<> auto lpp::of<Circle> = lpp::type_list<double>;
template<> auto lpp::of<Rectangle> = lpp::type_list<double, double>;
template<> auto lpp::of<Shape> = [](int shape_id) // <-- Can be of any type*
{
    switch (shape_id)
    {
    case 1: return lpp::extraction_info<Shape>::of<Circle>();    
    case 2: return lpp::extraction_info<Shape>::of<Rectangle>();    
    }
    throw std::invalid_argument("Invalid shape");
};

int main()
{
    Shape circle = lpp::cast("1 5.0", ' ');
    Shape rectangle = lpp::cast("2 10.0 50.0", ' ');
    
    assert(std::holds_alternative<Circle>(circle));
    assert(std::holds_alternative<Rectangle>(rectangle));
}
// *Provided that type has its own `lpp::of` specialization
```

