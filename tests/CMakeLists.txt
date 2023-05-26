add_executable(test
        test.cpp
        )

target_link_libraries(
        test PRIVATE
        project_warnings
        ${CONAN_LIBS}
)

gtest_discover_tests(test)