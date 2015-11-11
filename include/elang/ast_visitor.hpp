#ifndef ELANG_BASE_VISITOR_H
#define ELANG_BASE_VISITOR_H

#include <elang/ast.hpp>

namespace elang {
namespace ast {

class Visitor {
  public:
    virtual void visit(BinaryOperator* node) = 0;
    virtual void visit(UnaryOperator* node) = 0;
    virtual void visit(SubscriptExpression* node) = 0;
    virtual void visit(CallExpression* node) = 0;
    virtual void visit(CastExpression* node) = 0;
    virtual void visit(LValueToRValueCastExpression* node) = 0;
    virtual void visit(IdentifierReference* node) = 0;
    virtual void visit(IntLiteral* node) = 0;
    virtual void visit(DoubleLiteral* node) = 0;
    virtual void visit(CharLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(BoolLiteral* node) = 0;
    virtual void visit(CompoundStatement* node) = 0;
    virtual void visit(LetStatement* node) = 0;
    virtual void visit(ExpressionStatement* node) = 0;
    virtual void visit(SelectionStatement* node) = 0;
    virtual void visit(IterationStatement* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
    virtual void visit(FunctionDeclaration* node) = 0;
    virtual void visit(FunctionDefinition* node) = 0;
    virtual void visit(Module* node) = 0;
};

} // namespace ast
} // namespace elang

#endif // ELANG_BASE_VISITOR_H
