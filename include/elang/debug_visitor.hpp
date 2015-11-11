#ifndef ELANG_AST_DEBUG_VISITOR_H
#define ELANG_AST_DEBUG_VISITOR_H

#include <elang/ast_visitor.hpp>

namespace elang {
namespace ast {

class DebugVisitor : public Visitor {
    std::string current_tab;
    void increaseTab();
    void decreaseTab();

  public:
    virtual void visit(BinaryOperator* node) override;
    virtual void visit(UnaryOperator* node) override;
    virtual void visit(SubscriptExpression* node) override;
    virtual void visit(CallExpression* node) override;
    virtual void visit(CastExpression* node) override;
    virtual void visit(LValueToRValueCastExpression* node) override;
    virtual void visit(IdentifierReference* node) override;
    virtual void visit(IntLiteral* node) override;
    virtual void visit(DoubleLiteral* node) override;
    virtual void visit(CharLiteral* node) override;
    virtual void visit(StringLiteral* node) override;
    virtual void visit(BoolLiteral* node) override;
    virtual void visit(CompoundStatement* node) override;
    virtual void visit(LetStatement* node) override;
    virtual void visit(ExpressionStatement* node) override;
    virtual void visit(SelectionStatement* node) override;
    virtual void visit(IterationStatement* node) override;
    virtual void visit(ReturnStatement* node) override;
    virtual void visit(FunctionDeclaration* node) override;
    virtual void visit(FunctionDefinition* node) override;
    virtual void visit(Module* node) override;
};

} // namespace ast
} // namespace elang

#endif // ELANG_AST_DEBUG_VISITOR_H
