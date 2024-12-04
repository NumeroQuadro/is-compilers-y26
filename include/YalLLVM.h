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
#include "llvm/IR/Verifier.h"

class YalLLVM {
public:
    YalLLVM() { moduleInit(); }

    void exec(const std::string &program) {
        // 1.parse the program
        // auto ast = parser->parser(program);

        // 2. Compile to LLVM IR
        // compile(ast);

        // Print generated code
        module->print(llvm::outs(), nullptr);

        // 3. Save module IR to file
        saveModuleToFile("./out.ll");
    }

private:
    llvm::Function* fn;
    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    void compile() {
        fn = createFunction("main", llvm::FunctionType::get(builder->getInt32Ty(), false));
        auto result = gen(/*TODO: ast*/);

        auto i32Result = 
            builder->CreateIntCast(result, builder->getInt32Ty(), true);

        builder->CreateRet(i32Result);
    }

    llvm::Value* gen(/*TODO: exp*/) { 
        //return builder->getInt32(42);
        return builder->CreateGlobalStringPtr("Hello world string \n"); 
    }

    llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType) {
        auto fn = module->getFunction(fnName);

        if (fn == nullptr) {
            fn = createFunctionProto(fnName, fnType);
        }

        createFunctionBlock(fn);

        return fn;
    }

    llvm::Function* createFunctionProto(const std::string& fnName, llvm::FunctionType* fnType) {
        auto fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *module);
        llvm::verifyFunction(*fn);

        return fn;
    }

    void createFunctionBlock(llvm::Function* fn) {
        auto entry = createBB("entry", fn);
        builder->SetInsertPoint(entry);
    }

    llvm::BasicBlock* createBB(std::string name, llvm::Function* fn = nullptr) {
        return llvm::BasicBlock::Create(*ctx, name, fn);
    }

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

#endif // YalLLVM_H