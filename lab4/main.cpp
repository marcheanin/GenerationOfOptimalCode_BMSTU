#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

#include "Parser/Parser.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Expressions.hpp"
#include "IRGeneration/IRGenerator.hpp"
#include "IRGeneration/GraphVizPrinter.hpp"

int main(int argc, const char * argv[]) {
    if (argc > 1)
    {
        FILE * fp = freopen(argv[1], "r", stdin);
        if (fp == NULL)
        {
            perror(argv[1]);
            exit(1);
        }
    }

    // bool shouldUseLLVM = false;
    bool shouldPringGraphViz = true;
    
    Lexer *lexer = new Lexer();
    Parser *parser = new Parser(lexer);
    std::vector<AbstractExpression *> expressions = parser->Parse();
    
    
    IRGenerator irGenerator = IRGenerator();
    for (std::vector<AbstractExpression *>::iterator it = expressions.begin(); it != expressions.end(); ++it)
        irGenerator.GenerateIR((*it));


    irGenerator.CommitBuildingAndDump();
    if (shouldPringGraphViz) {
        irGenerator.GetGraphVizPrinter().print();
    }

    return 0;
}
