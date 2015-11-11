#include <elang/ast.hpp>

#include <cassert>

#include <elang/ast_visitor.hpp>

namespace elang {
namespace ast {

Node::Node(SourceLocation loc) : location(loc) {
}

Expression::Expression(SourceLocation loc) : Node(loc) {
}

BinaryOperator::BinaryOperator(BinaryOperator::Kind kind,
                               std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs,
                               SourceLocation loc)
    : Expression(loc), kind(kind), lhs(std::move(lhs)), rhs(std::move(rhs)) {
}

bool BinaryOperator::isComputable() {
    return lhs->isComputable() && rhs->isComputable();
}

UnaryOperator::UnaryOperator(UnaryOperator::Kind kind,
                             std::unique_ptr<Expression> expr,
                             SourceLocation loc)
    : Expression(loc), kind(kind), expr(std::move(expr)) {
}

bool UnaryOperator::isComputable() {
    return expr->isComputable();
}

SubscriptExpression::SubscriptExpression(
    std::unique_ptr<Expression> subscripted, std::unique_ptr<Expression> index,
    SourceLocation loc)
    : Expression(loc), subscripted(std::move(subscripted)),
      index(std::move(index)) {
}

bool SubscriptExpression::isComputable() {
    return subscripted->isComputable() && index->isComputable();
}

CallExpression::CallExpression(std::unique_ptr<IdentifierReference> func,
                               std::vector<std::unique_ptr<Expression>> args,
                               SourceLocation loc)
    : Expression(loc), func(std::move(func)), args(std::move(args)) {
}

bool CallExpression::isComputable() {
    return false;
}

CastExpression::CastExpression(std::unique_ptr<Expression> casted,
                               Type* to_type, SourceLocation loc)
    : Expression(loc), casted(std::move(casted)), to_type(to_type) {
}

bool CastExpression::isComputable() {
    return casted->isComputable();
}

LValueToRValueCastExpression::LValueToRValueCastExpression(
    std::unique_ptr<Expression> lvalue, SourceLocation loc)
    : Expression(loc), lvalue(std::move(lvalue)) {
}

bool LValueToRValueCastExpression::isComputable() {
    return lvalue->isComputable();
}

IdentifierReference::IdentifierReference(std::string name,
                                         std::vector<std::string> module_path,
                                         SourceLocation loc)
    : Expression(loc), name(name), module_path(std::move(module_path)) {
}

bool IdentifierReference::isComputable() {
    return false;
}

IntLiteral::IntLiteral(unsigned long value, SourceLocation loc)
    : Expression(loc), value(value) {
}

bool IntLiteral::isComputable() {
    return true;
}

DoubleLiteral::DoubleLiteral(double value, SourceLocation loc)
    : Expression(loc), value(value) {
}

bool DoubleLiteral::isComputable() {
    return true;
}

CharLiteral::CharLiteral(char value, SourceLocation loc)
    : Expression(loc), value(value) {
}

bool CharLiteral::isComputable() {
    return true;
}

StringLiteral::StringLiteral(std::string value, SourceLocation loc)
    : Expression(loc), value(value) {
}

bool StringLiteral::isComputable() {
    return true;
}

BoolLiteral::BoolLiteral(bool value, SourceLocation loc)
    : Expression(loc), value(value) {
}

bool BoolLiteral::isComputable() {
    return true;
}

Statement::Statement(SourceLocation loc) : Node(loc) {
}

CompoundStatement::CompoundStatement(
    std::vector<std::unique_ptr<Statement>> stmts, SourceLocation loc)
    : Statement(loc), stmts(std::move(stmts)) {
}

LetStatement::LetStatement(Type* type, std::string name,
                           std::unique_ptr<ast::Expression> init_expr,
                           SourceLocation loc)
    : Statement(loc), type(type), name(std::move(name)),
      init_expr(std::move(init_expr)) {
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr,
                                         SourceLocation loc)
    : Statement(loc), expr(std::move(expr)) {
}

SelectionStatement::SelectionStatement(
    std::vector<std::pair<std::unique_ptr<Expression>,
                          std::unique_ptr<CompoundStatement>>> choices,
    std::unique_ptr<CompoundStatement> else_stmt, SourceLocation loc)
    : Statement(loc), choices(std::move(choices)),
      else_stmt(std::move(else_stmt)) {
}

IterationStatement::IterationStatement(std::unique_ptr<Expression> condition,
                                       std::unique_ptr<CompoundStatement> stmt,
                                       SourceLocation loc)
    : Statement(loc), condition(std::move(condition)), stmt(std::move(stmt)) {
}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> expr,
                                 SourceLocation loc)
    : Statement(loc), expr(std::move(expr)) {
}

Declaration::Declaration(SourceLocation loc) : Node(loc) {
}

FunctionDeclaration::FunctionDeclaration(std::string name, FunctionType* type,
                                         SourceLocation loc)
    : Declaration(loc), name(std::move(name)), type(type) {
}

FunctionDefinition::FunctionDefinition(
    std::string name, FunctionType* type, std::vector<std::string> param_names,
    std::unique_ptr<CompoundStatement> content_stmt, SourceLocation loc)
    : FunctionDeclaration(name, type, loc), param_names(std::move(param_names)),
      content_stmt(std::move(content_stmt)) {
    assert(type->params_types.size() == this->param_names.size());
}

Module::Module(std::string name,
               std::vector<std::unique_ptr<Declaration>> declarations,
               SourceLocation loc)
    : Declaration(loc), name(name), declarations(std::move(declarations)) {
}

/* generation of visitors accept functions */
#define GENERATE_VISITOR(cls)                                                  \
    void cls::accept(Visitor* visitor) {                                       \
        visitor->visit(this);                                                  \
    }

GENERATE_VISITOR(BinaryOperator)
GENERATE_VISITOR(UnaryOperator)
GENERATE_VISITOR(SubscriptExpression)
GENERATE_VISITOR(CallExpression)
GENERATE_VISITOR(CastExpression)
GENERATE_VISITOR(LValueToRValueCastExpression)
GENERATE_VISITOR(IdentifierReference)
GENERATE_VISITOR(IntLiteral)
GENERATE_VISITOR(DoubleLiteral)
GENERATE_VISITOR(CharLiteral)
GENERATE_VISITOR(StringLiteral)
GENERATE_VISITOR(BoolLiteral)
GENERATE_VISITOR(CompoundStatement)
GENERATE_VISITOR(LetStatement)
GENERATE_VISITOR(ExpressionStatement)
GENERATE_VISITOR(SelectionStatement)
GENERATE_VISITOR(IterationStatement)
GENERATE_VISITOR(ReturnStatement)
GENERATE_VISITOR(FunctionDeclaration)
GENERATE_VISITOR(FunctionDefinition)
GENERATE_VISITOR(Module)

} // namespace ast
} // namespace elang
