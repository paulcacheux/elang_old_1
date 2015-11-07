#include <elang/ast.hpp>

#include <cassert>

#include <elang/ast_visitor.hpp>

namespace elang {
namespace ast {

BinaryOperator::BinaryOperator(BinaryOperator::Kind kind,
                               std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs)
    : kind(kind), lhs(std::move(lhs)), rhs(std::move(rhs)) {
}

UnaryOperator::UnaryOperator(UnaryOperator::Kind kind,
                             std::unique_ptr<Expression> expr)
    : kind(kind), expr(std::move(expr)) {
}

SubscriptExpression::SubscriptExpression(
    std::unique_ptr<Expression> subscripted, std::unique_ptr<Expression> index)
    : subscripted(std::move(subscripted)), index(std::move(index)) {
}

CallExpression::CallExpression(std::unique_ptr<IdentifierReference> func,
                               std::vector<std::unique_ptr<Expression>> args)
    : func(std::move(func)), args(std::move(args)) {
}

CastExpression::CastExpression(std::unique_ptr<Expression> casted,
                               Type* to_type)
    : casted(std::move(casted)), to_type(to_type) {
}

LValueToRValueCastExpression::LValueToRValueCastExpression(
    std::unique_ptr<Expression> lvalue)
    : lvalue(std::move(lvalue)) {
}

IdentifierReference::IdentifierReference(std::string name,
                                         std::vector<std::string> module_path)
    : name(name), module_path(std::move(module_path)) {
}

IntLiteral::IntLiteral(unsigned long value) : value(value) {
}

DoubleLiteral::DoubleLiteral(double value) : value(value) {
}

CharLiteral::CharLiteral(char value) : value(value) {
}

StringLiteral::StringLiteral(std::string value) : value(value) {
}

BoolLiteral::BoolLiteral(bool value) : value(value) {
}

CompoundStatement::CompoundStatement(
    std::vector<std::unique_ptr<Statement>> stmts)
    : stmts(std::move(stmts)) {
}

LetStatement::LetStatement(Type* type, std::string name,
                           std::unique_ptr<ast::Expression> init_expr)
    : type(type), name(std::move(name)), init_expr(std::move(init_expr)) {
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr)
    : expr(std::move(expr)) {
}

SelectionStatement::SelectionStatement(
    std::vector<std::pair<std::unique_ptr<Expression>,
                          std::unique_ptr<CompoundStatement>>> choices,
    std::unique_ptr<CompoundStatement> else_stmt)
    : choices(std::move(choices)), else_stmt(std::move(else_stmt)) {
}

IterationStatement::IterationStatement(std::unique_ptr<Expression> condition,
                                       std::unique_ptr<CompoundStatement> stmt)
    : condition(std::move(condition)), stmt(std::move(stmt)) {
}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> expr)
    : expr(std::move(expr)) {
}

ImportDeclaration::ImportDeclaration(std::string path) : path(path) {
}

FunctionDeclaration::FunctionDeclaration(std::string name, FunctionType* type)
    : name(std::move(name)), type(type) {
}

ExternFunctionDeclaration::ExternFunctionDeclaration(std::string name,
                                                     FunctionType* type)
    : FunctionDeclaration(name, type) {
}

FunctionDefinition::FunctionDefinition(
    std::string name, FunctionType* type, std::vector<std::string> param_names,
    std::unique_ptr<CompoundStatement> content_stmt)
    : FunctionDeclaration(name, type), param_names(std::move(param_names)),
      content_stmt(std::move(content_stmt)) {
    assert(type->params_types.size() == this->param_names.size());
}

TranslationUnit::TranslationUnit(
    std::vector<std::unique_ptr<ImportDeclaration>> imports,
    std::vector<std::unique_ptr<FunctionDeclaration>> func_decls)
    : imports(std::move(imports)), func_decls(std::move(func_decls)) {
}

Module::Module(std::string name, std::vector<std::unique_ptr<Module>> imp_mod,
               std::vector<std::unique_ptr<FunctionDeclaration>> funcs)
    : name(name), imported_modules(std::move(imp_mod)),
      functions(std::move(funcs)) {
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
GENERATE_VISITOR(ImportDeclaration)
GENERATE_VISITOR(FunctionDeclaration)
GENERATE_VISITOR(ExternFunctionDeclaration)
GENERATE_VISITOR(FunctionDefinition)
GENERATE_VISITOR(TranslationUnit)
GENERATE_VISITOR(Module)

} // namespace ast
} // namespace elang
