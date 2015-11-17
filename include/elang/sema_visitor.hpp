#ifndef ELANG_AST_SEMA_VISITOR_H
#define ELANG_AST_SEMA_VISITOR_H

#include <vector>
#include <string>

#include <elang/ast_visitor.hpp>
#include <elang/symbol_table.hpp>
#include <elang/op_inferer.hpp>

namespace elang {

class SourceManager;
class TypeManager;
class Type;
class DiagnosticEngine;

namespace ast {

class SemaVisitor : public Visitor {
    TypeManager* _type_manager;
    DiagnosticEngine* _diag_engine;
    OpInferer _op_inferer;
    std::unique_ptr<LocalTable> _local_table;
    GlobalTable _global_table;

    Type* _current_return_ty;

  public:
    explicit SemaVisitor(SourceManager* sm);

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

#endif // ELANG_AST_SEMA_VISITOR_H
