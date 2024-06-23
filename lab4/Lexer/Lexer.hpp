#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <vector>
class Token;

class Lexer {
public:
    std::vector<Token *>getAllTokens();
    Token* GetNextToken();
    Token* current;
private:
    Token* GetToken();
    int lastChar = ' ';
};
#endif /* Lexer_hpp */
