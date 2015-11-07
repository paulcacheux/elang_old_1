#include <elang/sema_visitor.hpp>

namespace elang {
namespace ast {

void visit(BinaryOperator* node) {
    if (node->kind != BinaryOperator::Kind::Assign) {
        // TODO: add the work here
    }
}

void visit(UnaryOperator* /*node*/) {
}

void visit(SubscriptExpression* /*node*/) {
}

void visit(CallExpression* /*node*/) {
}

void visit(CastExpression* /*node*/) {
}

void visit(LValueToRValueCastExpression* /*node*/) {
}

void visit(IdentifierReference* /*node*/) {
}

void visit(IntLiteral* /*node*/) {
}

void visit(DoubleLiteral* /*node*/) {
}

void visit(CharLiteral* /*node*/) {
}

void visit(StringLiteral* /*node*/) {
}

void visit(BoolLiteral* /*node*/) {
}

void visit(CompoundStatement* /*node*/) {
}

void visit(LetStatement* /*node*/) {
}

void visit(ExpressionStatement* /*node*/) {
}

void visit(SelectionStatement* /*node*/) {
}

void visit(IterationStatement* /*node*/) {
}

void visit(ReturnStatement* /*node*/) {
}

void visit(ImportDeclaration* /*node*/) {
}

void visit(FunctionDeclaration* /*node*/) {
}

void visit(ExternFunctionDeclaration* /*node*/) {
}

void visit(FunctionDefinition* /*node*/) {
}

void visit(TranslationUnit* /*node*/) {
}

void visit(Module* /*node*/) {
}

} // namespace ast
} // namespace elang
