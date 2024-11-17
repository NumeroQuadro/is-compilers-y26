cmake_minimum_required(VERSION 3.16)
project(MyLLVMProject)

# Include FetchContent module
include(FetchContent)

# Fetch LLVM source code
FetchContent_Declare(
  llvm-project
  GIT_REPOSITORY https://github.com/llvm/llvm-project.git
  GIT_TAG llvmorg-16.0.0 # Replace with the desired LLVM version
)

# Make the content available
FetchContent_MakeAvailable(llvm-project)

# Define build directory for LLVM
set(LLVM_BUILD_DIR ${CMAKE_BINARY_DIR}/llvm-build) # Build directory
set(CMAKE_BUILD_TYPE "Release") # Build type for LLVM and your project

# Configure LLVM build
set(LLVM_ENABLE_PROJECTS "clang;lld") # Projects to enable
set(LLVM_TARGETS_TO_BUILD "X86") # Only build X86 target

# Add LLVM subdirectory to build it
add_subdirectory(${llvm-project_SOURCE_DIR}/llvm ${LLVM_BUILD_DIR})

# Include LLVM headers
include_directories(${llvm-project_SOURCE_DIR}/llvm/include)
include_directories(${LLVM_BUILD_DIR}/include)

# Define your project's executable
add_executable(yal main.cpp)

# Link your project with LLVM libraries
target_link_libraries(yal PRIVATE LLVMCore LLVMSupport)
target_include_directories(yal PRIVATE ${CMAKE_SOURCE_DIR}/include)
