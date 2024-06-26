#include "GraphVizPrinter.hpp"
#include "../ControlFlowGraph/ControlFlowGraph.hpp"
#include "../ControlFlowGraph/BasicBlock.hpp"

void GraphVizPrinter::print() {
    printf("digraph G {\n");
    for (auto bb : _cfg->basicBlocks) {
        for (auto succ : bb->succs) {
            printf("\t\"%s\" -> \"%s\"\n", bb->stringValue().c_str(), succ->stringValue().c_str());
        }
    }
    printf("}\n");
}
