add_executable(test
        test.cpp
        )

add_executable(bench
        main.cpp
        test.hpp)

target_link_libraries(
        test PRIVATE
        project_warnings
        ${CONAN_LIBS}
)

gtest_discover_tests(test)