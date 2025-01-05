include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

FetchContent_GetProperties(googletest)
if (NOT googletest_POPULATED)
    FetchContent_MakeAvailable(googletest)
endif ()

enable_testing()

add_executable(run_tests Antrl4Tests.cpp
        RunTests.h)

target_link_libraries(run_tests
        PRIVATE gtest gtest_main
        PRIVATE Grammar_lib
        PRIVATE Core_lib
)

add_test(NAME MyTests COMMAND run_tests)
