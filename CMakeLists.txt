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
    FetchContent_Populate(antlr4)
    add_subdirectory(
            ${antlr4_SOURCE_DIR}/runtime/Cpp
            ${antlr4_BINARY_DIR}/runtime
            EXCLUDE_FROM_ALL
    )
endif()

set(ANTLR_JAR ${CMAKE_BINARY_DIR}/antlr-4.13.0-complete.jar)
file(DOWNLOAD
        https://www.antlr.org/download/antlr-4.13.0-complete.jar
        ${ANTLR_JAR}
        EXPECTED_HASH SHA256=bc6f4abc0d225a27570126c51402569f000a8deda3487b70e7642840e570e4a6
        SHOW_PROGRESS
)

function(antlr4_generate NAME GRAMMAR)
    set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/generated/${NAME})

    set(GENERATED_SOURCES
            ${OUTPUT_DIR}/${GRAMMAR}Lexer.cpp
            ${OUTPUT_DIR}/${GRAMMAR}Parser.cpp
    )

    add_custom_command(
            OUTPUT ${GENERATED_SOURCES}
            COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_JAR}
            -Dlanguage=Cpp -listener -visitor
            -o ${OUTPUT_DIR}
            ${SRC_DIR}/${GRAMMAR}.g4
            DEPENDS ${SRC_DIR}/${GRAMMAR}.g4
            COMMENT "Running ANTLR4 on ${GRAMMAR}.g4"
    )

    add_custom_target(${NAME}_antlr4 ALL DEPENDS ${GENERATED_SOURCES})
    add_library(${NAME}_lib STATIC ${GENERATED_SOURCES})
    target_include_directories(${NAME}_lib PUBLIC
            ${OUTPUT_DIR}
            ${antlr4_SOURCE_DIR}/runtime/Cpp/runtime/src
    )
    target_link_libraries(${NAME}_lib PUBLIC antlr4_static)

    set(${NAME}_GENERATED_SOURCES ${GENERATED_SOURCES} PARENT_SCOPE)
endfunction()

antlr4_generate(MyGrammar Grammar)

add_library(Grammar_lib STATIC ${MyGrammar_GENERATED_SOURCES})
target_include_directories(Grammar_lib PUBLIC
        ${CMAKE_BINARY_DIR}/generated/MyGrammar
        ${antlr4_SOURCE_DIR}/runtime/Cpp/runtime/src
)

target_link_libraries(Grammar_lib PUBLIC antlr4_static)

add_library(Core_lib STATIC
        include/GrammarASTInterpreter.cpp
        include/Value.cpp
        include/VirtualMachine.cpp
        include/Scope.cpp
        include/GarbageCollector.cpp
        include/GarbageCollector.h
        include/HeapValue.cpp
        include/HeapValue.h
)

target_include_directories(Core_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/generated/MyGrammar
        ${ANTLR4_INCLUDE_DIR}
)

target_link_libraries(Core_lib PUBLIC Grammar_lib)

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
