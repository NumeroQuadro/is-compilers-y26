include(FetchContent)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v2.2.0
)

FetchContent_MakeAvailable(cxxopts)

add_executable(yal
        main.cpp
)

target_include_directories(yal PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/generated/MyGrammar
        ${ANTLR4_INCLUDE_DIR}
)

target_link_libraries(yal PRIVATE Core_lib cxxopts)
