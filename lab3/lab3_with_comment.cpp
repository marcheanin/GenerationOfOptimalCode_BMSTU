```cpp
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Instruction.h"
#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace llvm;

// Перечисление различных типов токенов
enum Token {
    token_error = 0x4000,
    token_eof = 0x2000,
    token_ident = 0x1000,
    token_number = 0x0800,
    token_if = 0x0400,
    token_for = 0x0200,
    token_return = 0x0100,
    token_lparen = 0x0080,
    token_rparen = 0x0040,
    token_lfig = 0x0020,
    token_rfig = 0x0010,
    token_comma = 0x0008,
    token_eq = 0x0004,
    token_plus = 0x0002, 
    token_minus = 0x0001,
    token_else = 0x0000
};

static std::string input_seq;
static int current_position = 0;
static std::string StrVal;
static int NumVal;

// Функция для получения следующего символа из входной последовательности
static char next_char() {
    if (current_position >= input_seq.length())
        return '$';  // Специальный символ, означающий конец файла
    char c = input_seq[current_position++];
    return c;
}

// Функция для получения следующего токена из входной последовательности
static Token get_token() {
    static char LastChar = ' ';
   
    // Пропуск пробелов
    while (isspace(LastChar))
        LastChar = next_char();

    // Обработка идентификаторов и ключевых слов
    if (isalpha(LastChar)) {
        StrVal = LastChar;
        while (isalnum((LastChar = next_char())))
            StrVal += LastChar;

        if (StrVal == "if")
            return token_if;
        if (StrVal == "for")
            return token_for;
        if (StrVal == "return")
            return token_return;
        if (StrVal == "else")
            return token_else;

        return token_ident;
    }

    // Обработка чисел
    if (isdigit(LastChar)) {
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = next_char();
        } while (isdigit(LastChar));

        NumVal = strtol(NumStr.c_str(), nullptr, 10);
        return token_number;
    }

    // Обработка специальных символов
    Token t = token_error;
    switch (LastChar) {
        case '$': t = token_eof; break;
        case '(': t = token_lparen; break;
        case ')': t = token_rparen; break;
        case '{': t = token_lfig; break;
        case '}': t = token_rfig; break;
        case ',': t = token_comma; break;
        case '=': t = token_eq; break;
        case '+': t = token_plus; break;
        case '-': t =  token_minus; break;
    }
    LastChar = next_char();
    
    return t;
}

static Token current_token = token_eof;
static Token getNextToken() { return current_token = get_token(); }

// Функция для инициализации сканера с входным файлом
void init_scanner(const char* filepath) {
    current_position = 0;
    std::ifstream t(filepath);
    input_seq = std::string((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
}

static std::unique_ptr<LLVMContext> context;
static std::unique_ptr<Module> mod;
static std::unique_ptr<IRBuilder<>> builder;
static std::map<std::string, AllocaInst *> named_values;

// Функция для инициализации модуля LLVM
static void initialize_module() {
  context = std::make_unique<LLVMContext>();
  mod = std::make_unique<Module>("lab3", *context);
  builder = std::make_unique<IRBuilder<>>(*context);
}

// Абстрактный базовый класс для всех узлов выражений
class ExprAST {
public:
  virtual ~ExprAST() = default;

  // Метод для генерации кода для данного выражения
  virtual Value *codegen() = 0;
};

// Класс выражения для числовых литералов, таких как "1.0"
class NumberExprAST : public ExprAST {
  int Val;

public:
  NumberExprAST(int Val) : Val(Val) {}

  Value *codegen() override;
};

// Класс выражения для ссылки на переменную
class VariableExprAST : public ExprAST {
  std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
    const std::string &getName() const { return Name; }

    Value *codegen() override;
};

// Класс выражения для бинарного оператора
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  Value *codegen() override;
};

// Класс выражения для условного оператора if
class IfExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> Cond;
  std::vector<std::unique_ptr<ExprAST> > Then, Else;

public:
  IfExprAST(std::unique_ptr<ExprAST> Cond, std::vector<std::unique_ptr<ExprAST> > Then,
            std::vector<std::unique_ptr<ExprAST> > Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  Value *codegen() override;
};

// Класс выражения для цикла for
class ForExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Start, End, Step;
    std::vector<std::unique_ptr<ExprAST> > Body;

public:
  ForExprAST(std::unique_ptr<ExprAST> Start,
             std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
             std::vector<std::unique_ptr<ExprAST> > Body)
      : Start(std::move(Start)), End(std::move(End)),
        Step(std::move(Step)), Body(std::move(Body)) {}

  Value *codegen() override;
};

// Класс, представляющий "прототип" функции
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
      : Name(Name), Args(std::move(Args)) {}

  Function *codegen();
  const std::string &getName() const { return Name; }
};

// Класс, представляющий определение функции
class FunctionAST {
public:
  std::unique_ptr<PrototypeAST> proto;
  std::vector<std::unique_ptr<ExprAST>> body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST>& proto,
              std::vector<std::unique_ptr<ExprAST>>& body)
      : proto(std::move(proto)), body(std::move(body)) {}

  Function *codegen();
};

// Вспомогательная функция для создания инструкции alloca в начальном блоке функции
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          StringRef VarName) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getInt32Ty(*context), nullptr, VarName);
}

// Генерация кода для числовых литералов
Value *NumberExprAST::codegen() {
    return ConstantInt::get(*context, APInt(32, Val, false));
}

// Генерация кода для ссылок на переменные
Value *VariableExprAST::codegen() {
  AllocaInst *A = named_values[Name];
  if (!A)
    A = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Name);
    named_values[Name] = A;
  return builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
}

// Генерация кода для бинарных операторов
Value *BinaryExprAST::codegen() {
    if (Op == '=') {
        VariableExprAST *LHSE = static_cast<VariableExprAST *>(LHS.get());
        if (!LHSE)
            return nullptr;

        Value *Val = RHS->codegen();
        if (!Val)
            return nullptr;
        
        Value *Variable = named_values[LHSE->getName()];
        if (!Variable) {
            Variable = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), LH

SE->getName());
            named_values[LHSE->getName()] = static_cast<AllocaInst*>(Variable);
        }
            
        builder->CreateStore(Val, Variable);
        return Val;
    }

    Value *L = LHS->codegen();
    Value *R = RHS->codegen();
    if (!L || !R)
        return nullptr;

    switch (Op) {
        case '+':
            return builder->CreateAdd(L, R, "addtmp");
        case '-':
            return builder->CreateSub(L, R, "subtmp");
        default:
            return nullptr;
    }
}
```