cmake_minimum_required(VERSION 3.6)
project(yal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Java 1.11 COMPONENTS Runtime REQUIRED)

include(FetchContent)
FetchContent_Declare(
    antlr4
    GIT_REPOSITORY https://github.com/antlr/antlr4.git
    GIT_TAG 4.13.0
)
FetchContent_GetProperties(antlr4)
if(NOT antlr4_POPULATED)
    FetchContent_MakeAvailable(antlr4)
endif()


find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(include)
add_subdirectory(bin)
add_subdirectory(tests)

set(MESSAGE "
************************************
*                                  *
*  You know what they say          *
*  If Young Metro don't trust you  *
*  Motherf***er, you better run    *
*                                  *
************************************
")
message(STATUS "${MESSAGE}")
