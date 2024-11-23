cmake_minimum_required(VERSION 3.16)
project(yal)
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_executable(yal main.cpp)
target_include_directories(yal PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)
llvm_map_components_to_libnames(llvm_libs support core irreader nativecodegen)
target_link_libraries(yal ${llvm_libs})