# Добавление исполняемого файла
add_executable(yal main.cpp)

# Подключение директорий и библиотек
target_include_directories(yal PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated/MyGrammar # Include directory for generated files
    ${ANTLR4_INCLUDE_DIR}                   # Include directory for ANTLR runtime
)
llvm_map_components_to_libnames(llvm_libs support core irreader nativecodegen)
target_link_libraries(
    yal 
    PRIVATE Grammar_lib
    PRIVATE ${llvm_libs}
)
