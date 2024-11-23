# Загрузка ANTLR JAR файла
set(ANTLR_JAR ${CMAKE_BINARY_DIR}/antlr-4.13.0-complete.jar)

file(DOWNLOAD
    https://www.antlr.org/download/antlr-4.13.0-complete.jar
    ${ANTLR_JAR}
    EXPECTED_HASH SHA256=bc6f4abc0d225a27570126c51402569f000a8deda3487b70e7642840e570e4a6
    SHOW_PROGRESS
)

# Установка путей для ANTLR runtime
set(ANTLR4_RUNTIME_DIR ${antlr4_SOURCE_DIR}/runtime/Cpp)
set(ANTLR4_INCLUDE_DIR ${ANTLR4_RUNTIME_DIR}/runtime/src)
set(ANTLR4_LIB antlr4_static)

# Добавление ANTLR4 библиотек
add_subdirectory(${ANTLR4_RUNTIME_DIR} ${CMAKE_BINARY_DIR}/antlr4)

# Функция для генерации C++ кода из грамматики ANTLR
function(antlr4_generate NAME GRAMMAR)
    set(SRC_DIR ${CMAKE_SOURCE_DIR}/src) # Корректный путь к src
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/generated/${NAME})

    # Путь к сгенерированным исходникам
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
        ${OUTPUT_DIR}           # Include directory for generated files
        ${ANTLR4_INCLUDE_DIR}   # Include directory for ANTLR runtime
    )

    set(${NAME}_GENERATED_SOURCES ${GENERATED_SOURCES} PARENT_SCOPE)
endfunction()

# Генерация грамматики
antlr4_generate(MyGrammar Grammar)

# Создание статической библиотеки
add_library(Grammar_lib STATIC ${MyGrammar_GENERATED_SOURCES})

# Включение директорий с заголовками
target_include_directories(Grammar_lib PUBLIC 
    ${CMAKE_BINARY_DIR}/generated/MyGrammar # Include directory for generated files
    ${ANTLR4_INCLUDE_DIR}                   # Include directory for ANTLR runtime
)
target_link_libraries(Grammar_lib PUBLIC ${ANTLR4_LIB})
