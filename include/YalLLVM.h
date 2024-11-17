#ifndef YalLLVM_h
#define YalLLVM_h

#include <iostream>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

class YalLLVM {
    public:
    YalLLVM() { moduleInit(); }

    void exec(const std::string &program) {
        // 1.parse the program
        // auto ast = parser->parser(program);

        // 2. Compile to LLVM IR
        // compile(ast)

        // Print generated code
        module->print(llvm::outs(), nullptr);

        // 3. Save module IR to file
        saveModuleToFile("./out.ll");
    }

    private:
        std::unique_ptr<llvm::LLVMContext> ctx;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<llvm::IRBuilder<>> builder;

        void saveModuleToFile(const std::string& filename) {
            std::error_code errorCode;
            llvm::raw_fd_ostream outLL(filename, errorCode);
            module->print(outLL, nullptr);
        }

        void moduleInit() {
            ctx = std::make_unique<llvm::LLVMContext>();
            module = std::make_unique<llvm::Module>("YalLLVM", *ctx);
            builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
        }
};

#endif