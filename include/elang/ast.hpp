#ifndef ELANG_AST_H
#define ELANG_AST_H

#include <memory>
#include <string>
#include <vector>

#include <elang/type.hpp>

namespace elang {
namespace ast {

// useful forward decl
class Visitor;
class VarDecl;
class IdentifierReference;

class Node {
  public:
    virtual void accept(Visitor* visitor) = 0;
};

class Expression : public Node {
  public:
    virtual void accept(Visitor* visitor) = 0;
};

class BinaryOperator : public Expression {
  public:
    enum class Kind {
        Assign,
        LessOrEqual,
        Less,
        Greater,
        GreaterOrEqual,
        Equal,
        Different,
        Add,
        Minus,
        Times,
        Divide,
        Modulo,
        LogicalAnd,
        LogicalOr
    };

    BinaryOperator(Kind kind, std::unique_ptr<Expression> lhs,
                   std::unique_ptr<Expression> rhs);
    virtual void accept(Visitor* visitor) override;

    Kind kind;
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
};

class UnaryOperator : public Expression {
  public:
    enum class Kind { Plus, Minus, LogicalNot, PtrDeref, AddressOf };

    UnaryOperator(Kind kind, std::unique_ptr<Expression> expr);
    virtual void accept(Visitor* visitor) override;

    Kind kind;
    std::unique_ptr<Expression> expr;
};

class SubscriptExpression : public Expression {
  public:
    SubscriptExpression(std::unique_ptr<Expression> subscripted,
                        std::unique_ptr<Expression> index);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> subscripted;
    std::unique_ptr<Expression> index;
};

class CallExpression : public Expression {
  public:
    CallExpression(std::unique_ptr<IdentifierReference> func,
                   std::vector<std::unique_ptr<Expression>> args);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<IdentifierReference> func;
    std::vector<std::unique_ptr<Expression>> args;
};

class CastExpression : public Expression {
  public:
    CastExpression(std::unique_ptr<Expression> casted, Type* to_type);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> casted;
    Type* to_type;
};

class LValueToRValueCastExpression : public Expression {
  public:
    explicit LValueToRValueCastExpression(std::unique_ptr<Expression> lvalue);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> lvalue;
};

class IdentifierReference : public Expression {
  public:
    explicit IdentifierReference(std::string name,
                                 std::vector<std::string> module_path);
    virtual void accept(Visitor* visitor) override;

    std::string name;
    std::vector<std::string> module_path;
};

class IntLiteral : public Expression {
  public:
    explicit IntLiteral(unsigned long value);
    virtual void accept(Visitor* visitor) override;

    unsigned long value;
};

class DoubleLiteral : public Expression {
  public:
    explicit DoubleLiteral(double value);
    virtual void accept(Visitor* visitor) override;

    double value;
};

class CharLiteral : public Expression {
  public:
    explicit CharLiteral(char value);
    virtual void accept(Visitor* visitor) override;

    char value;
};

class StringLiteral : public Expression {
  public:
    explicit StringLiteral(std::string value);
    virtual void accept(Visitor* visitor) override;

    std::string value;
};

class BoolLiteral : public Expression {
  public:
    explicit BoolLiteral(bool value);
    virtual void accept(Visitor* visitor) override;

    bool value;
};

class Statement : public Node {
  public:
    virtual void accept(Visitor* visitor) = 0;
};

class CompoundStatement : public Statement {
  public:
    CompoundStatement(std::vector<std::unique_ptr<Statement>> stmts);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::unique_ptr<Statement>> stmts;
};

class LetStatement : public Statement {
  public:
    LetStatement(Type* type, std::string name,
                 std::unique_ptr<ast::Expression> init_expr = nullptr);
    virtual void accept(Visitor* visitor) override;

    Type* type;
    std::string name;
    std::unique_ptr<ast::Expression> init_expr;
};

class ExpressionStatement : public Statement {

  public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expr = nullptr);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> expr;
};

class SelectionStatement : public Statement {
  public:
    SelectionStatement(
        std::vector<std::pair<std::unique_ptr<Expression>,
                              std::unique_ptr<CompoundStatement>>> choices,
        std::unique_ptr<CompoundStatement> else_stmt = nullptr);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::pair<std::unique_ptr<Expression>,
                          std::unique_ptr<CompoundStatement>>> choices;
    std::unique_ptr<CompoundStatement> else_stmt;
};

class IterationStatement : public Statement {
  public:
    IterationStatement(std::unique_ptr<Expression> condition,
                       std::unique_ptr<CompoundStatement> stmt);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> condition;
    std::unique_ptr<CompoundStatement> stmt;
};

class ReturnStatement : public Statement {
  public:
    explicit ReturnStatement(std::unique_ptr<Expression> expr = nullptr);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> expr;
};

class ImportDeclaration : public Node {
  public:
    explicit ImportDeclaration(std::string path);
    virtual void accept(Visitor* visitor) override;

    std::string path;
};

class FunctionDeclaration : public Node {
  public:
    FunctionDeclaration(std::string name, FunctionType* type);
    virtual void accept(Visitor* visitor) override;

    std::string name;
    FunctionType* type;
};

class ExternFunctionDeclaration : public FunctionDeclaration {
  public:
    ExternFunctionDeclaration(std::string name, FunctionType* type);
    virtual void accept(Visitor* visitor) override;
};

class FunctionDefinition : public FunctionDeclaration {
  public:
    FunctionDefinition(std::string name, FunctionType* type,
                       std::vector<std::string> param_names,
                       std::unique_ptr<CompoundStatement> content_stmt);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::string> param_names;
    std::unique_ptr<CompoundStatement> content_stmt;
};

class TranslationUnit : public Node {
  public:
    TranslationUnit(
        std::vector<std::unique_ptr<ImportDeclaration>> imports,
        std::vector<std::unique_ptr<FunctionDeclaration>> func_decls);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::unique_ptr<ImportDeclaration>> imports;
    std::vector<std::unique_ptr<FunctionDeclaration>> func_decls;
};

class Module : public Node {
  public:
    std::string name;
    std::vector<std::unique_ptr<Module>> imported_modules;
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;

    Module(std::string name, std::vector<std::unique_ptr<Module>> imp_mod,
           std::vector<std::unique_ptr<FunctionDeclaration>> funcs);
    virtual void accept(Visitor* visitor) override;
};

} // namespace ast
} // namespace elang

#endif // ELANG_AST_H
