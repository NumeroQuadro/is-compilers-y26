## Fetch Google Test
#include(FetchContent)
#
#FetchContent_Declare(
#    googletest
#    GIT_REPOSITORY https://github.com/google/googletest.git
#    GIT_TAG release-1.12.1
#)
#
## Ensure Google Test is populated
#FetchContent_GetProperties(googletest)
#if(NOT googletest_POPULATED)
#    FetchContent_MakeAvailable(googletest)
#endif()
#
## Enable testing
#enable_testing()
#
## Add a test executable
#add_executable(run_tests Antrl4Tests.cpp)
#
## Link libraries to the test executable
#target_link_libraries(run_tests
#    PRIVATE gtest gtest_main # Use gtest_main for the default main function
#    PRIVATE Grammar_lib      # Link your custom library
#)
#
## Add the tests to CTest
#add_test(NAME MyTests COMMAND run_tests)
