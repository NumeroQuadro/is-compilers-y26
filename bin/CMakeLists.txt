add_executable(yal main.cpp
        GrammarASTInterpreter.h
        GrammarASTInterpreter.cpp
        Value.cpp
        Value.h
        Interpreter.cpp
        Interpreter.h
        VirtualMachine.cpp
        VirtualMachine.h
        Instruction.h
        MyStack.h
        Scope.h
        Scope.cpp
        CallFrame.h
        FunctionInfo.h)

target_include_directories(yal PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated/MyGrammar
    ${ANTLR4_INCLUDE_DIR}
)
llvm_map_components_to_libnames(llvm_libs support core irreader nativecodegen)
target_link_libraries(
    yal 
    PRIVATE Grammar_lib
    PRIVATE ${llvm_libs}
)

add_custom_command(
        TARGET yal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/src/example.txt
                ${CMAKE_CURRENT_BINARY_DIR}/src/example.txt)