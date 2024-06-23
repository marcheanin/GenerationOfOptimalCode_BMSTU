#ifndef GraphVizPrinter_hpp
#define GraphVizPrinter_hpp

#include <stdio.h>

class ControlFlowGraph;


class GraphVizPrinter {
    ControlFlowGraph *_cfg;
    
public:
    void print();
    GraphVizPrinter(ControlFlowGraph *cfg) : _cfg(cfg) {}
};

#endif /* GraphVizPrinter_hpp */
