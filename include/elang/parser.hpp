#ifndef ELANG_PARSER_H
#define ELANG_PARSER_H

#include <memory>
#include <vector>

#include <elang/ast.hpp>
#include <elang/type.hpp>
#include <elang/token.hpp>

namespace elang {

class Lexer;
class SourceManager;
class DiagnosticEngine;

class Parser {
    Lexer* _lexer;
    TypeManager* _type_manager;
    DiagnosticEngine* _diag_engine;

  public:
    Parser(Lexer* lexer, SourceManager* sm);
    std::unique_ptr<ast::Module> parseMainModule();

  private:
    std::unique_ptr<ast::Declaration> parseDeclaration();
    std::unique_ptr<ast::Module> parseModule();
    std::unique_ptr<ast::FunctionDeclaration> parseFunctionDeclaration();
    Type* parseQualType();
    BuiltinType* parseBuiltinType();
    std::pair<std::vector<std::string>, std::vector<Type*>> readParams();

    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::LetStatement> parseLetStatement();
    std::unique_ptr<ast::CompoundStatement> parseCompoundStatement();
    std::unique_ptr<ast::SelectionStatement> parseSelectionStatement();
    std::unique_ptr<ast::IterationStatement> parseIterationStatement();
    std::unique_ptr<ast::ReturnStatement> parseReturnStatement();
    std::unique_ptr<ast::ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<ast::Expression> parseExpression();
    std::unique_ptr<ast::Expression> parseLogicalOrExpression();
    std::unique_ptr<ast::Expression> parseLogicalAndExpression();
    std::unique_ptr<ast::Expression> parseSimpleExpression();
    std::unique_ptr<ast::Expression> parseAddExpression();
    std::unique_ptr<ast::Expression> parseTermExpression();
    std::unique_ptr<ast::Expression> parseCastExpression();
    std::unique_ptr<ast::Expression> parseUnaryExpression();
    std::unique_ptr<ast::Expression> parseSubscriptExpression();
    std::unique_ptr<ast::Expression> parseFactorExpression();
    std::unique_ptr<ast::IdentifierReference> parseIdentifierReference();
    std::vector<std::unique_ptr<ast::Expression>> parseArgs();

    void expect(Token::Kind kind);
    Token accept(Token::Kind kind);
};

} // namespace elang
#endif // ELANG_PARSER_H
