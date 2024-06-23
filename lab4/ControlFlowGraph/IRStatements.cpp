#include "IRStatements.hpp"
#include "../Parser/Expressions.hpp"
#include "BasicBlock.hpp"

std::string AssignStatement::Dump() {
    return var->stringValue() + " = " + rhs->stringValue();
}

std::string BranchStatement::Dump() {
    if (isConditional) {
        return "branch on: " + condition->stringValue() + " to: " + firstBranchBB->stringValue() + " or: " + secondBranchBB->stringValue();
    } else {
        return "branch to: " + firstBranchBB->stringValue();
    }
}

std::string PhiNodeStatement::Dump() {
    std::string argEnumeration;
    for (auto arg : bbToVarMap) {
        argEnumeration += arg.second->stringValue() + " " + arg.first->stringValue() + "; ";
    }
    return var->stringValue() + " = [" + argEnumeration + "]";
}
