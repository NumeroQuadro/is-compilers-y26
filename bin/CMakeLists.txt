add_executable(yal
        main.cpp
)

target_include_directories(yal PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/generated/MyGrammar
        ${ANTLR4_INCLUDE_DIR}
)

target_link_libraries(yal PRIVATE Core_lib)
