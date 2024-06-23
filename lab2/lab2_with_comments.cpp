#include "llvm/ADT/APInt.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/IRBuilder.h"

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

int main() {

    // Создание контекста LLVM
    LLVMContext context;
    // Создание нового модуля с именем "gen-opt-lab2"
    Module *module = new Module("gen-opt-lab2", context); 
    // Создание IRBuilder для генерации LLVM IR
    IRBuilder<> builder(context); 
    // Создание типа функции, возвращающей 32-битное целое число
    FunctionType *funcType = FunctionType::get(builder.getInt32Ty(), false); 
    
    // Создание функции "main" с внешним связыванием
    Function *mainFunc = Function::Create(
        funcType, 
        Function::ExternalLinkage,
        "main",
        module); 

    // Создание базового блока "entrypoint" внутри функции "main"
    BasicBlock *entry = BasicBlock::Create(
        context, 
        "entrypoint", 
        mainFunc);

    // Установка текущей точки вставки в базовый блок "entrypoint"
    builder.SetInsertPoint(entry); 

    // Создание константы 140 типа i32
    Value *a_const = ConstantInt::get(
        Type::getInt32Ty(context), 
        140);
    // Создание константы 67 типа i32
    Value *b_const = ConstantInt::get(
        Type::getInt32Ty(context), 
        67); 
    // Создание инструкции сложения констант и сохранение результата в "retVal"
    Value *return_value = builder.CreateAdd(a_const, b_const, "retVal"); 
    // Создание инструкции возврата значения "retVal"
    builder.CreateRet(return_value);  

    // Вывод сгенерированного LLVM IR на стандартный вывод
    module->dump();

    // Освобождение памяти, занятой модулем
    delete module;
    return 0;
}