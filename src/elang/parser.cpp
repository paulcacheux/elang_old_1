#include <elang/parser.hpp>

#include <elang/source_manager.hpp>
#include <elang/lexer.hpp>
#include <elang/diagnoctic.hpp>

namespace elang {

Parser::Parser(Lexer* lexer, SourceManager* sm)
    : _lexer(lexer), _type_manager(sm->getTypeManager()),
      _diag_engine(sm->getDiagnosticEngine()) {
}

std::unique_ptr<ast::TranslationUnit> Parser::parseTranslationUnit() {
    std::vector<std::unique_ptr<ast::ImportDeclaration>> imports;
    while (_lexer->peekToken().is(Token::Kind::kw_import)) {
        imports.push_back(parseImportDeclaration());
    }

    std::vector<std::unique_ptr<ast::FunctionDeclaration>> func_decls;
    while (_lexer->peekToken().isNot(Token::Kind::eof)) {
        func_decls.push_back(parseFunctionDeclaration());
    }
    return std::make_unique<ast::TranslationUnit>(std::move(imports),
                                                  std::move(func_decls));
}

std::unique_ptr<ast::ImportDeclaration> Parser::parseImportDeclaration() {
    expect(Token::Kind::kw_import);
    // TODO change to string
    auto import = accept(Token::Kind::identifier).value;
    return std::make_unique<ast::ImportDeclaration>(import);
}

std::unique_ptr<ast::FunctionDeclaration> Parser::parseFunctionDeclaration() {
    bool is_extern = false;
    if (isNext(Token::Kind::kw_extern))
        is_extern = true;

    expect(Token::Kind::kw_func);
    auto name = accept(Token::Kind::identifier).value;
    expect(Token::Kind::l_paren);
    auto params = readParams();
    expect(Token::Kind::r_paren);

    Type* ret_type = _type_manager->getVoidType();
    if (isNext(Token::Kind::arrow)) {
        ret_type = parseQualType();
    }

    FunctionType* func_ty
        = _type_manager->getFunctionType(ret_type, params.second);

    if (is_extern) {
        expect(Token::Kind::semi);
        return std::make_unique<ast::ExternFunctionDeclaration>(name, func_ty);
    }

    if (isNext(Token::Kind::semi)) {
        return std::make_unique<ast::FunctionDeclaration>(name, func_ty);
    }

    auto content_stmt = parseCompoundStatement();
    return std::make_unique<ast::FunctionDefinition>(
        name, func_ty, params.first, std::move(content_stmt));
}

Type* Parser::parseQualType() {
    if (isNext(Token::Kind::l_square)) {
        auto subtype = parseQualType();
        expect(Token::Kind::semi);
        auto size = std::stoll(accept(Token::Kind::int_literal).value);
        expect(Token::Kind::r_square);
        return _type_manager->getArrayType(subtype, size);
    } else if (isNext(Token::Kind::star)) {
        auto subtype = parseQualType();
        return _type_manager->getPointerType(subtype);
    } else {
        return parseBuiltinType();
    }
}

BuiltinType* Parser::parseBuiltinType() {
    if (isNext(Token::Kind::kw_int)) {
        return _type_manager->getIntType();
    } else if (isNext(Token::Kind::kw_void)) {
        return _type_manager->getVoidType();
    } else if (isNext(Token::Kind::kw_bool)) {
        return _type_manager->getBoolType();
    } else if (isNext(Token::Kind::kw_char)) {
        return _type_manager->getCharType();
    } else {
        _diag_engine->report(ErrorLevel::Error, _lexer->getToken().location,
                             "Unexpected token");

        return parseBuiltinType();
    }
}

std::pair<std::vector<std::string>, std::vector<Type*>> Parser::readParams() {
    std::vector<std::string> names;
    std::vector<Type*> types;

    if (_lexer->peekToken().isNot(Token::Kind::r_paren)) {
        auto name = accept(Token::Kind::identifier).value;
        expect(Token::Kind::colon);
        auto type = parseQualType();

        names.push_back(name);
        types.push_back(type);

        while (_lexer->peekToken().is(Token::Kind::comma)) {
            _lexer->getToken();
            auto name = accept(Token::Kind::identifier).value;
            expect(Token::Kind::colon);
            auto type = parseQualType();

            names.push_back(name);
            types.push_back(type);
        }
    }
    return std::make_pair(names, types);
}

std::unique_ptr<ast::Statement> Parser::parseStatement() {
    if (_lexer->peekToken().is(Token::Kind::kw_let)) {
        return std::move(parseLetStatement());
    } else if (_lexer->peekToken().is(Token::Kind::kw_if)) {
        return std::move(parseSelectionStatement());
    } else if (_lexer->peekToken().is(Token::Kind::kw_while)) {
        return std::move(parseIterationStatement());
    } else if (_lexer->peekToken().is(Token::Kind::kw_return)) {
        return std::move(parseReturnStatement());
    } else if (_lexer->peekToken().is(Token::Kind::l_brace)) {
        return std::move(parseCompoundStatement());
    } else {
        return std::move(parseExpressionStatement());
    }
}

std::unique_ptr<ast::LetStatement> Parser::parseLetStatement() {
    expect(Token::Kind::kw_let);
    auto name = accept(Token::Kind::identifier).value;
    expect(Token::Kind::colon);
    auto type = parseQualType();

    std::unique_ptr<ast::Expression> init_expr{nullptr};
    if (isNext(Token::Kind::equal)) {
        init_expr = parseExpression();
    }

    expect(Token::Kind::semi);
    return std::make_unique<ast::LetStatement>(type, name,
                                               std::move(init_expr));
}

std::unique_ptr<ast::CompoundStatement> Parser::parseCompoundStatement() {
    expect(Token::Kind::l_brace);
    std::vector<std::unique_ptr<ast::Statement>> stmts;
    while (_lexer->peekToken().isNot(Token::Kind::r_brace)) {
        stmts.push_back(std::move(parseStatement()));
    }
    expect(Token::Kind::r_brace);
    return std::make_unique<ast::CompoundStatement>(std::move(stmts));
}

std::unique_ptr<ast::SelectionStatement> Parser::parseSelectionStatement() {
    expect(Token::Kind::kw_if);
    auto condition = parseExpression();
    auto stmt = parseCompoundStatement();

    std::vector<std::pair<std::unique_ptr<ast::Expression>,
                          std::unique_ptr<ast::CompoundStatement>>> choices;
    choices.emplace_back(std::move(condition), std::move(stmt));

    std::unique_ptr<ast::CompoundStatement> else_stmt{nullptr};
    while (isNext(Token::Kind::kw_else)) {
        if (isNext(Token::Kind::kw_if)) {
            auto condition = parseExpression();
            auto stmt = parseCompoundStatement();
            choices.emplace_back(std::move(condition), std::move(stmt));
        } else {
            else_stmt = parseCompoundStatement();
            break;
        }
    }
    return std::make_unique<ast::SelectionStatement>(std::move(choices),
                                                     std::move(else_stmt));
}

std::unique_ptr<ast::IterationStatement> Parser::parseIterationStatement() {
    expect(Token::Kind::kw_while);
    auto condition = parseExpression();
    auto stmt = parseCompoundStatement();
    return std::make_unique<ast::IterationStatement>(std::move(condition),
                                                     std::move(stmt));
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {
    expect(Token::Kind::kw_return);
    std::unique_ptr<ast::Expression> expr{nullptr};
    if (_lexer->peekToken().isNot(Token::Kind::semi)) {
        expr = parseExpression();
    }
    expect(Token::Kind::semi);
    return std::make_unique<ast::ReturnStatement>(std::move(expr));
}

std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement() {
    std::unique_ptr<ast::Expression> expr{nullptr};
    if (_lexer->peekToken().isNot(Token::Kind::semi)) {
        expr = parseExpression();
    }
    expect(Token::Kind::semi);
    return std::make_unique<ast::ExpressionStatement>(std::move(expr));
}

std::unique_ptr<ast::Expression> Parser::parseExpression() {
    auto rhs_expr = parseLogicalOrExpression();
    if (isNext(Token::Kind::equal)) {
        auto lhs_expr = parseExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            ast::BinaryOperator::Kind::Assign, std::move(rhs_expr),
            std::move(lhs_expr));
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseLogicalOrExpression() {
    auto rhs_expr = parseLogicalAndExpression();
    while (isNext(Token::Kind::pipepipe)) {
        auto lhs_expr = parseLogicalAndExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            ast::BinaryOperator::Kind::LogicalOr, std::move(rhs_expr),
            std::move(lhs_expr));
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseLogicalAndExpression() {
    auto rhs_expr = parseSimpleExpression();
    while (isNext(Token::Kind::ampamp)) {
        auto lhs_expr = parseSimpleExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            ast::BinaryOperator::Kind::LogicalAnd, std::move(rhs_expr),
            std::move(lhs_expr));
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseSimpleExpression() {
    auto rhs_expr = parseAddExpression();
    if (_lexer->peekToken().isOneOf(
            Token::Kind::lessequal, Token::Kind::less, Token::Kind::greater,
            Token::Kind::greaterequal, Token::Kind::equalequal,
            Token::Kind::exclaimequal)) {
        ast::BinaryOperator::Kind kind;
        auto tok_kind = _lexer->getToken().kind;
        if (tok_kind == Token::Kind::lessequal)
            kind = ast::BinaryOperator::Kind::LessOrEqual;
        else if (tok_kind == Token::Kind::less)
            kind = ast::BinaryOperator::Kind::Less;
        else if (tok_kind == Token::Kind::greater)
            kind = ast::BinaryOperator::Kind::Greater;
        else if (tok_kind == Token::Kind::greaterequal)
            kind = ast::BinaryOperator::Kind::GreaterOrEqual;
        else if (tok_kind == Token::Kind::equalequal)
            kind = ast::BinaryOperator::Kind::Equal;
        else if (tok_kind == Token::Kind::exclaimequal)
            kind = ast::BinaryOperator::Kind::Different;

        auto lhs_expr = parseAddExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            kind, std::move(rhs_expr), std::move(lhs_expr));
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseAddExpression() {
    auto rhs_expr = parseTermExpression();
    while (_lexer->peekToken().isOneOf(Token::Kind::plus, Token::Kind::minus)) {
        ast::BinaryOperator::Kind kind;
        auto tok_kind = _lexer->getToken().kind;
        if (tok_kind == Token::Kind::plus)
            kind = ast::BinaryOperator::Kind::Add;
        else if (tok_kind == Token::Kind::minus)
            kind = ast::BinaryOperator::Kind::Minus;

        auto lhs_expr = parseTermExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            kind, std::move(rhs_expr), std::move(lhs_expr));
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseTermExpression() {
    auto rhs_expr = parseCastExpression();
    while (_lexer->peekToken().isOneOf(Token::Kind::star, Token::Kind::slash,
                                       Token::Kind::percent)) {
        ast::BinaryOperator::Kind kind;
        auto tok_kind = _lexer->getToken().kind;
        if (tok_kind == Token::Kind::star)
            kind = ast::BinaryOperator::Kind::Times;
        else if (tok_kind == Token::Kind::slash)
            kind = ast::BinaryOperator::Kind::Divide;
        else if (tok_kind == Token::Kind::percent)
            kind = ast::BinaryOperator::Kind::Modulo;

        auto lhs_expr = parseCastExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            kind, std::move(rhs_expr), std::move(lhs_expr));
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseCastExpression() {
    auto casted = parseUnaryExpression();
    if (isNext(Token::Kind::kw_as)) {
        auto to_type = parseQualType();
        return std::make_unique<ast::CastExpression>(std::move(casted),
                                                     to_type);
    }
    return std::move(casted);
}

std::unique_ptr<ast::Expression> Parser::parseUnaryExpression() {
    if (_lexer->peekToken().isOneOf(Token::Kind::plus, Token::Kind::minus,
                                    Token::Kind::exclaim, Token::Kind::star,
                                    Token::Kind::amp)) {
        ast::UnaryOperator::Kind kind;
        auto tok_kind = _lexer->getToken().kind;
        if (tok_kind == Token::Kind::plus)
            kind = ast::UnaryOperator::Kind::Plus;
        else if (tok_kind == Token::Kind::minus)
            kind = ast::UnaryOperator::Kind::Minus;
        else if (tok_kind == Token::Kind::exclaim)
            kind = ast::UnaryOperator::Kind::LogicalNot;
        else if (tok_kind == Token::Kind::star)
            kind = ast::UnaryOperator::Kind::PtrDeref;
        else if (tok_kind == Token::Kind::amp)
            kind = ast::UnaryOperator::Kind::AddressOf;

        auto expr = parseSubscriptExpression();
        return std::make_unique<ast::UnaryOperator>(kind, std::move(expr));
    }
    return parseSubscriptExpression();
}

std::unique_ptr<ast::Expression> Parser::parseSubscriptExpression() {
    auto expr = parseFactorExpression();
    while (isNext(Token::Kind::l_square)) {
        auto index = parseExpression();
        expr = std::make_unique<ast::SubscriptExpression>(std::move(expr),
                                                          std::move(index));
        expect(Token::Kind::r_square);
    }
    return std::move(expr);
}

std::unique_ptr<ast::Expression> Parser::parseFactorExpression() {
    if (isNext(Token::Kind::l_paren)) {
        auto expr = parseExpression();
        expect(Token::Kind::r_paren);
        return std::move(expr);
    } else if (_lexer->peekToken().is(Token::Kind::int_literal)) {
        return std::make_unique<ast::IntLiteral>(
            std::stoll(_lexer->getToken().value));
    } else if (_lexer->peekToken().is(Token::Kind::char_literal)) {
        return std::make_unique<ast::CharLiteral>(
            _lexer->getToken().value.front());
    } else if (_lexer->peekToken().is(Token::Kind::double_literal)) {
        return std::make_unique<ast::DoubleLiteral>(
            std::stod(_lexer->getToken().value));
    } else if (_lexer->peekToken().is(Token::Kind::string_literal)) {
        return std::make_unique<ast::StringLiteral>(_lexer->getToken().value);
    } else if (_lexer->peekToken().is(Token::Kind::boolean_literal)) {
        return std::make_unique<ast::BoolLiteral>(_lexer->getToken().value
                                                  == "true");
    } else {
        auto id_expr = parseIdentifierReference();
        if (_lexer->peekToken().is(Token::Kind::l_paren)) {
            _lexer->getToken();
            auto args = parseArgs();
            expect(Token::Kind::r_paren);
            return std::make_unique<ast::CallExpression>(std::move(id_expr),
                                                         std::move(args));
        }
        return std::move(id_expr);
    }
}

std::unique_ptr<ast::IdentifierReference> Parser::parseIdentifierReference() {
    auto identifier_name = accept(Token::Kind::identifier).value;
    std::vector<std::string> module_path;
    while (isNext(Token::Kind::coloncolon)) {
        module_path.push_back(identifier_name);
        identifier_name = accept(Token::Kind::identifier).value;
    }
    return std::make_unique<ast::IdentifierReference>(identifier_name,
                                                      module_path);
}

std::vector<std::unique_ptr<ast::Expression>> Parser::parseArgs() {
    std::vector<std::unique_ptr<ast::Expression>> args;
    if (_lexer->peekToken().isNot(Token::Kind::r_paren)) {
        args.push_back(std::move(parseExpression()));
        while (isNext(Token::Kind::comma)) {
            args.push_back(std::move(parseExpression()));
        }
    }
    return std::move(args);
}

bool Parser::isNext(Token::Kind kind) {
    auto result = _lexer->peekToken().is(kind);
    if (result) {
        _lexer->getToken();
    }
    return result;
}

void Parser::expect(Token::Kind kind) {
    while (!_lexer->peekToken().isOneOf(kind, Token::Kind::eof)) {
        _diag_engine->report(ErrorLevel::Error, _lexer->getToken().location,
                             "Unexpected token");
    }
    _lexer->getToken();
}

Token Parser::accept(Token::Kind kind) {
    while (!_lexer->peekToken().isOneOf(kind, Token::Kind::eof)) {
        _diag_engine->report(ErrorLevel::Error, _lexer->getToken().location,
                             "Unexpected token");
    }
    return _lexer->getToken();
}

} // namespace elang