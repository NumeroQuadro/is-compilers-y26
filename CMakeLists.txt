cmake_minimum_required(VERSION 3.6)
project(yal)

# Укажите минимальную версию Java, необходимую для ANTLR4
find_package(Java 1.11 COMPONENTS Runtime REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Установка пути к JAR файлу ANTLR
set(ANTLR_JAR ${CMAKE_BINARY_DIR}/antlr-4.13.0-complete.jar)

# Скачиваем ANTLR JAR файл
file(DOWNLOAD
        https://www.antlr.org/download/antlr-4.13.0-complete.jar
        ${ANTLR_JAR}
        EXPECTED_HASH SHA256=bc6f4abc0d225a27570126c51402569f000a8deda3487b70e7642840e570e4a6
        SHOW_PROGRESS
)

# Подключение ANTLR4 в проекте
include(FetchContent)
FetchContent_Declare(
        antlr4
        GIT_REPOSITORY https://github.com/antlr/antlr4.git
        GIT_TAG 4.13.0
        SOURCE_DIR ${CMAKE_BINARY_DIR}/antlr4-src
        BINARY_DIR ${CMAKE_BINARY_DIR}/antlr4-build
)
FetchContent_GetProperties(antlr4)
if(NOT antlr4_POPULATED)
    FetchContent_MakeAvailable(antlr4)
endif()

# Настройка переменных для ANTLR4
set(ANTLR4_RUNTIME_DIR ${antlr4_SOURCE_DIR}/runtime/Cpp)
set(ANTLR4_INCLUDE_DIR ${ANTLR4_RUNTIME_DIR}/runtime/src)
set(ANTLR4_LIB antlr4_static)

# Добавление ANTLR4 библиотек
add_subdirectory(${ANTLR4_RUNTIME_DIR} ${CMAKE_BINARY_DIR}/antlr4)

# Включение директорий ANTLR4
include_directories(${ANTLR4_INCLUDE_DIR})
link_libraries(${ANTLR4_LIB})

# Функция для генерации C++ кода из грамматики ANTLR
function(antlr4_generate NAME GRAMMAR)
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/generated/${NAME})

    # Путь к сгенерированным исходникам
    set(GENERATED_SOURCES
            ${OUTPUT_DIR}/${GRAMMAR}Lexer.cpp
            ${OUTPUT_DIR}/${GRAMMAR}Parser.cpp
    )

    # Команда генерации исходников
    add_custom_command(
            OUTPUT ${GENERATED_SOURCES}
            COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_JAR}
            -Dlanguage=Cpp -listener -visitor
            -o ${OUTPUT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/${GRAMMAR}.g4
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/${GRAMMAR}.g4
            COMMENT "Running ANTLR4 on ${GRAMMAR}.g4"
    )

    add_custom_target(${NAME}_antlr4 ALL DEPENDS ${GENERATED_SOURCES})
    add_library(${NAME}_lib STATIC ${GENERATED_SOURCES}) # Создаем библиотеку из сгенерированных исходников
    target_include_directories(${NAME}_lib PUBLIC ${OUTPUT_DIR}) # Добавляем директорию с хедерами

    set(${NAME}_GENERATED_SOURCES ${GENERATED_SOURCES} PARENT_SCOPE)
endfunction()

antlr4_generate(MyGrammar Grammar)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(yal bin/main.cpp)

target_include_directories(yal PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs support core irreader nativecodegen)
target_link_libraries(
    yal 
    PRIVATE ${llvm_libs}
    PRIVATE MyGrammar_lib ${ANTLR4_LIB}
    PRIVATE ${ANTLR4_LIB}
)
