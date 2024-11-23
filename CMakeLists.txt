cmake_minimum_required(VERSION 3.6)
project(yal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите минимальную версию Java, необходимую для ANTLR4
find_package(Java 1.11 COMPONENTS Runtime REQUIRED)

# Подключение ANTLR4 в проект
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

# LLVM настройки
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Добавление поддиректорий
add_subdirectory(include)
add_subdirectory(bin)

# Завершающее сообщение
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
