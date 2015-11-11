#ifndef ELANG_AST_H
#define ELANG_AST_H

#include <memory>
#include <string>
#include <vector>

#include <elang/source_location.hpp>
#include <elang/type.hpp>

namespace elang {
namespace ast {

// useful forward decl
class Visitor;
class VarDecl;
class IdentifierReference;

class Node {
  public:
    explicit Node(SourceLocation loc);
    virtual void accept(Visitor* visitor) = 0;

    SourceLocation location;
};

class Expression : public Node {
  public:
    explicit Expression(SourceLocation loc);
    virtual void accept(Visitor* visitor) = 0;
    virtual bool isComputable() = 0;

    Type* type{nullptr};
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
                   std::unique_ptr<Expression> rhs, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    Kind kind;
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
};

class UnaryOperator : public Expression {
  public:
    enum class Kind { Plus, Minus, LogicalNot, PtrDeref, AddressOf };

    UnaryOperator(Kind kind, std::unique_ptr<Expression> expr,
                  SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    Kind kind;
    std::unique_ptr<Expression> expr;
};

class SubscriptExpression : public Expression {
  public:
    SubscriptExpression(std::unique_ptr<Expression> subscripted,
                        std::unique_ptr<Expression> index, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    std::unique_ptr<Expression> subscripted;
    std::unique_ptr<Expression> index;
};

class CallExpression : public Expression {
  public:
    CallExpression(std::unique_ptr<IdentifierReference> func,
                   std::vector<std::unique_ptr<Expression>> args,
                   SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    std::unique_ptr<IdentifierReference> func;
    std::vector<std::unique_ptr<Expression>> args;
};

class CastExpression : public Expression {
  public:
    CastExpression(std::unique_ptr<Expression> casted, Type* to_type,
                   SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    std::unique_ptr<Expression> casted;
    Type* to_type;
};

class LValueToRValueCastExpression : public Expression {
  public:
    explicit LValueToRValueCastExpression(std::unique_ptr<Expression> lvalue,
                                          SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    std::unique_ptr<Expression> lvalue;
};

class IdentifierReference : public Expression {
  public:
    explicit IdentifierReference(std::string name,
                                 std::vector<std::string> module_path,
                                 SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    std::string name;
    std::vector<std::string> module_path;
};

class IntLiteral : public Expression {
  public:
    explicit IntLiteral(unsigned long value, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    unsigned long value;
};

class DoubleLiteral : public Expression {
  public:
    explicit DoubleLiteral(double value, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    double value;
};

class CharLiteral : public Expression {
  public:
    explicit CharLiteral(char value, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    char value;
};

class StringLiteral : public Expression {
  public:
    explicit StringLiteral(std::string value, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    std::string value;
};

class BoolLiteral : public Expression {
  public:
    explicit BoolLiteral(bool value, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;
    virtual bool isComputable() override;

    bool value;
};

class Statement : public Node {
  public:
    explicit Statement(SourceLocation loc);
    virtual void accept(Visitor* visitor) = 0;
};

class CompoundStatement : public Statement {
  public:
    CompoundStatement(std::vector<std::unique_ptr<Statement>> stmts,
                      SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::unique_ptr<Statement>> stmts;
};

class LetStatement : public Statement {
  public:
    LetStatement(Type* type, std::string name,
                 std::unique_ptr<ast::Expression> init_expr,
                 SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    Type* type;
    std::string name;
    std::unique_ptr<ast::Expression> init_expr;
};

class ExpressionStatement : public Statement {

  public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expr,
                                 SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> expr;
};

class SelectionStatement : public Statement {
  public:
    SelectionStatement(
        std::vector<std::pair<std::unique_ptr<Expression>,
                              std::unique_ptr<CompoundStatement>>> choices,
        std::unique_ptr<CompoundStatement> else_stmt, SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::pair<std::unique_ptr<Expression>,
                          std::unique_ptr<CompoundStatement>>> choices;
    std::unique_ptr<CompoundStatement> else_stmt;
};

class IterationStatement : public Statement {
  public:
    IterationStatement(std::unique_ptr<Expression> condition,
                       std::unique_ptr<CompoundStatement> stmt,
                       SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> condition;
    std::unique_ptr<CompoundStatement> stmt;
};

class ReturnStatement : public Statement {
  public:
    explicit ReturnStatement(std::unique_ptr<Expression> expr,
                             SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::unique_ptr<Expression> expr;
};

class Declaration : public Node {
  public:
    explicit Declaration(SourceLocation loc);
    virtual void accept(Visitor* visitor) = 0;
};

class FunctionDeclaration : public Declaration {
  public:
    FunctionDeclaration(std::string name, FunctionType* type,
                        SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::string name;
    FunctionType* type;
};

class FunctionDefinition : public FunctionDeclaration {
  public:
    FunctionDefinition(std::string name, FunctionType* type,
                       std::vector<std::string> param_names,
                       std::unique_ptr<CompoundStatement> content_stmt,
                       SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::vector<std::string> param_names;
    std::unique_ptr<CompoundStatement> content_stmt;
};

class Module : public Declaration {
  public:
    Module(std::string name,
           std::vector<std::unique_ptr<Declaration>> declarations,
           SourceLocation loc);
    virtual void accept(Visitor* visitor) override;

    std::string name;
    std::vector<std::unique_ptr<Declaration>> declarations;
};

} // namespace ast
} // namespace elang

#endif // ELANG_AST_H
