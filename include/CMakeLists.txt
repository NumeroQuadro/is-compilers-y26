set(ANTLR_JAR ${CMAKE_BINARY_DIR}/antlr-4.13.0-complete.jar)

file(DOWNLOAD
    https://www.antlr.org/download/antlr-4.13.0-complete.jar
    ${ANTLR_JAR}
    EXPECTED_HASH SHA256=bc6f4abc0d225a27570126c51402569f000a8deda3487b70e7642840e570e4a6
    SHOW_PROGRESS
)

set(ANTLR4_RUNTIME_DIR ${antlr4_SOURCE_DIR}/runtime/Cpp)
set(ANTLR4_INCLUDE_DIR ${ANTLR4_RUNTIME_DIR}/runtime/src)
set(ANTLR4_LIB antlr4_static)

add_subdirectory(${ANTLR4_RUNTIME_DIR} ${CMAKE_BINARY_DIR}/antlr4)

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
        -o ${OUTPUT_DIR} ${SRC_DIR}/${GRAMMAR}.g4
        DEPENDS ${SRC_DIR}/${GRAMMAR}.g4
        COMMENT "Running ANTLR4 on ${GRAMMAR}.g4"
    )

    add_custom_target(${NAME}_antlr4 ALL DEPENDS ${GENERATED_SOURCES})
    add_library(${NAME}_lib STATIC ${GENERATED_SOURCES})
    target_include_directories(${NAME}_lib PUBLIC 
        ${OUTPUT_DIR}
        ${ANTLR4_INCLUDE_DIR}
    )

    set(${NAME}_GENERATED_SOURCES ${GENERATED_SOURCES} PARENT_SCOPE)
endfunction()

antlr4_generate(MyGrammar Grammar)

add_library(Grammar_lib STATIC ${MyGrammar_GENERATED_SOURCES})

target_include_directories(Grammar_lib PUBLIC 
    ${CMAKE_BINARY_DIR}/generated/MyGrammar
    ${ANTLR4_INCLUDE_DIR}
)
target_link_libraries(Grammar_lib PUBLIC ${ANTLR4_LIB})
