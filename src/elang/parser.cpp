#include <elang/parser.hpp>

#include <elang/source_manager.hpp>
#include <elang/lexer.hpp>
#include <elang/diagnostic.hpp>

namespace elang {

Parser::Parser(Lexer* lexer, SourceManager* sm)
    : _lexer(lexer), _type_manager(sm->getTypeManager()),
      _diag_engine(sm->getDiagnosticEngine()) {
}

std::unique_ptr<ast::Module> Parser::parseMainModule() {
    auto loc = _lexer->peekToken().location;
    std::vector<std::unique_ptr<ast::Declaration>> declarations;
    while (_lexer->peekToken().isNot(Token::Kind::eof)) {
        declarations.push_back(parseDeclaration());
    }
    return std::make_unique<ast::Module>("", std::move(declarations), loc);
}

std::unique_ptr<ast::Declaration> Parser::parseDeclaration() {
    if (_lexer->peekToken().is(Token::Kind::kw_mod)) {
        return std::move(parseModule());
    } else if (_lexer->peekToken().is(Token::Kind::kw_func)) {
        return std::move(parseFunctionDeclaration());
    } else {
        auto tok = _lexer->getToken();
        _diag_engine->report(tok.location, 2001, tok.value);
        return parseDeclaration();
    }
}

std::unique_ptr<ast::Module> Parser::parseModule() {
    auto loc = accept(Token::Kind::kw_mod).location;
    auto name = accept(Token::Kind::identifier).value;
    expect(Token::Kind::l_brace);

    std::vector<std::unique_ptr<ast::Declaration>> declarations;
    while (_lexer->peekToken().isNot(Token::Kind::r_brace)) {
        declarations.push_back(parseDeclaration());
    }
    expect(Token::Kind::r_brace);
    return std::make_unique<ast::Module>(name, std::move(declarations), loc);
}

std::unique_ptr<ast::FunctionDeclaration> Parser::parseFunctionDeclaration() {
    auto loc = accept(Token::Kind::kw_func).location;
    auto name = accept(Token::Kind::identifier).value;
    expect(Token::Kind::l_paren);
    auto params = readParams();
    expect(Token::Kind::r_paren);

    Type* ret_type = _type_manager->getVoidType();
    if (_lexer->peekToken().is(Token::Kind::arrow)) {
        _lexer->getToken();
        ret_type = parseQualType();
    }

    FunctionType* func_ty
        = _type_manager->getFunctionType(ret_type, params.second);

    if (_lexer->peekToken().is(Token::Kind::semi)) {
        _lexer->getToken();
        return std::make_unique<ast::FunctionDeclaration>(name, func_ty, loc);
    }

    auto content_stmt = parseCompoundStatement();
    return std::make_unique<ast::FunctionDefinition>(
        name, func_ty, params.first, std::move(content_stmt), loc);
}

Type* Parser::parseQualType() {
    if (_lexer->peekToken().is(Token::Kind::l_square)) {
        _lexer->getToken();
        auto subtype = parseQualType();
        expect(Token::Kind::semi);
        auto size = std::stoll(accept(Token::Kind::int_literal).value);
        expect(Token::Kind::r_square);
        return _type_manager->getArrayType(subtype, size);
    } else if (_lexer->peekToken().is(Token::Kind::star)) {
        _lexer->getToken();
        auto subtype = parseQualType();
        return _type_manager->getPointerType(subtype);
    } else {
        return parseBuiltinType();
    }
}

BuiltinType* Parser::parseBuiltinType() {
    auto tok = _lexer->getToken();
    if (tok.is(Token::Kind::kw_int)) {
        return _type_manager->getIntType();
    } else if (tok.is(Token::Kind::kw_void)) {
        return _type_manager->getVoidType();
    } else if (tok.is(Token::Kind::kw_bool)) {
        return _type_manager->getBoolType();
    } else if (tok.is(Token::Kind::kw_char)) {
        return _type_manager->getCharType();
    } else if (tok.is(Token::Kind::kw_double)) {
        return _type_manager->getDoubleType();
    } else {
        _diag_engine->report(tok.location, 2001, tok.value);
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
    auto loc = accept(Token::Kind::kw_let).location;
    auto name = accept(Token::Kind::identifier).value;

    Type* type = nullptr;
    std::unique_ptr<ast::Expression> init_expr = nullptr;

    auto tok = _lexer->getToken();
    if (tok.is(Token::Kind::colon)) {
        type = parseQualType();
        if (_lexer->peekToken().is(Token::Kind::equal)) {
            _lexer->getToken();
            init_expr = parseExpression();
        }
    } else if (tok.is(Token::Kind::equal)) {
        init_expr = parseExpression();
    } else {
        _diag_engine->report(loc, 2002, tok.value);
    }

    expect(Token::Kind::semi);
    return std::make_unique<ast::LetStatement>(type, name, std::move(init_expr),
                                               loc);
}

std::unique_ptr<ast::CompoundStatement> Parser::parseCompoundStatement() {
    auto loc = accept(Token::Kind::l_brace).location;
    std::vector<std::unique_ptr<ast::Statement>> stmts;
    while (_lexer->peekToken().isNot(Token::Kind::r_brace)) {
        stmts.push_back(std::move(parseStatement()));
    }
    expect(Token::Kind::r_brace);
    return std::make_unique<ast::CompoundStatement>(std::move(stmts), loc);
}

std::unique_ptr<ast::SelectionStatement> Parser::parseSelectionStatement() {
    auto loc = accept(Token::Kind::kw_if).location;
    auto condition = parseExpression();
    auto stmt = parseCompoundStatement();

    std::vector<std::pair<std::unique_ptr<ast::Expression>,
                          std::unique_ptr<ast::CompoundStatement>>> choices;
    choices.emplace_back(std::move(condition), std::move(stmt));

    std::unique_ptr<ast::CompoundStatement> else_stmt{nullptr};
    while (_lexer->peekToken().is(Token::Kind::kw_else)) {
        _lexer->getToken();
        if (_lexer->peekToken().is(Token::Kind::kw_if)) {
            _lexer->getToken();
            auto condition = parseExpression();
            auto stmt = parseCompoundStatement();
            choices.emplace_back(std::move(condition), std::move(stmt));
        } else {
            else_stmt = parseCompoundStatement();
            break;
        }
    }
    return std::make_unique<ast::SelectionStatement>(std::move(choices),
                                                     std::move(else_stmt), loc);
}

std::unique_ptr<ast::IterationStatement> Parser::parseIterationStatement() {
    auto loc = accept(Token::Kind::kw_while).location;
    auto condition = parseExpression();
    auto stmt = parseCompoundStatement();
    return std::make_unique<ast::IterationStatement>(std::move(condition),
                                                     std::move(stmt), loc);
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {
    auto loc = accept(Token::Kind::kw_return).location;
    std::unique_ptr<ast::Expression> expr{nullptr};
    if (_lexer->peekToken().isNot(Token::Kind::semi)) {
        expr = parseExpression();
    }
    expect(Token::Kind::semi);
    return std::make_unique<ast::ReturnStatement>(std::move(expr), loc);
}

std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement() {
    std::unique_ptr<ast::Expression> expr{nullptr};
    SourceLocation loc = _lexer->peekToken().location; // just to initalize
    if (_lexer->peekToken().isNot(Token::Kind::semi)) {
        expr = parseExpression();
        loc = expr->location;
        expect(Token::Kind::semi);
    } else {
        loc = accept(Token::Kind::semi).location;
    }
    return std::make_unique<ast::ExpressionStatement>(std::move(expr), loc);
}

std::unique_ptr<ast::Expression> Parser::parseExpression() {
    auto rhs_expr = parseLogicalOrExpression();
    if (_lexer->peekToken().is(Token::Kind::equal)) {
        auto loc = _lexer->getToken().location;
        auto lhs_expr = parseExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            ast::BinaryOperator::Kind::Assign, std::move(rhs_expr),
            std::move(lhs_expr), loc);
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseLogicalOrExpression() {
    auto rhs_expr = parseLogicalAndExpression();
    while (_lexer->peekToken().is(Token::Kind::pipepipe)) {
        auto loc = _lexer->getToken().location;
        auto lhs_expr = parseLogicalAndExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            ast::BinaryOperator::Kind::LogicalOr, std::move(rhs_expr),
            std::move(lhs_expr), loc);
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseLogicalAndExpression() {
    auto rhs_expr = parseSimpleExpression();
    while (_lexer->peekToken().is(Token::Kind::ampamp)) {
        auto loc = _lexer->getToken().location;
        auto lhs_expr = parseSimpleExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            ast::BinaryOperator::Kind::LogicalAnd, std::move(rhs_expr),
            std::move(lhs_expr), loc);
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseSimpleExpression() {
    auto rhs_expr = parseAddExpression();
    if (_lexer->peekToken().isOneOf(
            Token::Kind::lessequal, Token::Kind::less, Token::Kind::greater,
            Token::Kind::greaterequal, Token::Kind::equalequal,
            Token::Kind::exclaimequal)) {
        auto loc = _lexer->peekToken().location;
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
            kind, std::move(rhs_expr), std::move(lhs_expr), loc);
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseAddExpression() {
    auto rhs_expr = parseTermExpression();
    while (_lexer->peekToken().isOneOf(Token::Kind::plus, Token::Kind::minus)) {
        auto loc = _lexer->peekToken().location;
        ast::BinaryOperator::Kind kind;
        auto tok_kind = _lexer->getToken().kind;
        if (tok_kind == Token::Kind::plus)
            kind = ast::BinaryOperator::Kind::Add;
        else if (tok_kind == Token::Kind::minus)
            kind = ast::BinaryOperator::Kind::Minus;

        auto lhs_expr = parseTermExpression();
        rhs_expr = std::make_unique<ast::BinaryOperator>(
            kind, std::move(rhs_expr), std::move(lhs_expr), loc);
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseTermExpression() {
    auto rhs_expr = parseCastExpression();
    while (_lexer->peekToken().isOneOf(Token::Kind::star, Token::Kind::slash,
                                       Token::Kind::percent)) {
        auto loc = _lexer->peekToken().location;
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
            kind, std::move(rhs_expr), std::move(lhs_expr), loc);
    }
    return std::move(rhs_expr);
}

std::unique_ptr<ast::Expression> Parser::parseCastExpression() {
    auto casted = parseUnaryExpression();
    if (_lexer->peekToken().is(Token::Kind::kw_as)) {
        auto loc = _lexer->getToken().location;
        auto to_type = parseQualType();
        return std::make_unique<ast::CastExpression>(std::move(casted), to_type,
                                                     loc);
    }
    return std::move(casted);
}

std::unique_ptr<ast::Expression> Parser::parseUnaryExpression() {
    if (_lexer->peekToken().isOneOf(Token::Kind::plus, Token::Kind::minus,
                                    Token::Kind::exclaim, Token::Kind::star,
                                    Token::Kind::amp)) {
        auto loc = _lexer->peekToken().location;
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
        return std::make_unique<ast::UnaryOperator>(kind, std::move(expr), loc);
    }
    return parseSubscriptExpression();
}

std::unique_ptr<ast::Expression> Parser::parseSubscriptExpression() {
    auto expr = parseFactorExpression();
    while (_lexer->peekToken().is(Token::Kind::l_square)) {
        auto loc = _lexer->getToken().location;
        auto index = parseExpression();
        expr = std::make_unique<ast::SubscriptExpression>(
            std::move(expr), std::move(index), loc);
        expect(Token::Kind::r_square);
    }
    return std::move(expr);
}

std::unique_ptr<ast::Expression> Parser::parseFactorExpression() {
    if (_lexer->peekToken().is(Token::Kind::l_paren)) {
        _lexer->getToken();
        auto expr = parseExpression();
        expect(Token::Kind::r_paren);
        return std::move(expr);
    } else if (_lexer->peekToken().is(Token::Kind::int_literal)) {
        auto tok = _lexer->getToken();
        return std::make_unique<ast::IntLiteral>(std::stoll(tok.value),
                                                 tok.location);
    } else if (_lexer->peekToken().is(Token::Kind::char_literal)) {
        auto tok = _lexer->getToken();
        return std::make_unique<ast::CharLiteral>(tok.value.front(),
                                                  tok.location);
    } else if (_lexer->peekToken().is(Token::Kind::double_literal)) {
        auto tok = _lexer->getToken();
        return std::make_unique<ast::DoubleLiteral>(std::stod(tok.value),
                                                    tok.location);
    } else if (_lexer->peekToken().is(Token::Kind::string_literal)) {
        auto tok = _lexer->getToken();
        return std::make_unique<ast::StringLiteral>(tok.value, tok.location);
    } else if (_lexer->peekToken().is(Token::Kind::boolean_literal)) {
        auto tok = _lexer->getToken();
        return std::make_unique<ast::BoolLiteral>(tok.value == "true",
                                                  tok.location);
    } else {
        auto id_expr = parseIdentifierReference();
        if (_lexer->peekToken().is(Token::Kind::l_paren)) {
            _lexer->getToken();
            auto args = parseArgs();
            auto loc = accept(Token::Kind::r_paren).location;
            return std::make_unique<ast::CallExpression>(std::move(id_expr),
                                                         std::move(args), loc);
        }
        return std::move(id_expr);
    }
}

std::unique_ptr<ast::IdentifierReference> Parser::parseIdentifierReference() {
    std::string identifier_name;
    std::vector<std::string> module_path;
    SourceLocation loc = _lexer->peekToken().location;

    if (_lexer->peekToken().is(Token::Kind::coloncolon)) {
        _lexer->getToken();
        module_path.push_back("");
    }

    auto tok = accept(Token::Kind::identifier);
    loc = tok.location;
    identifier_name = tok.value;

    while (_lexer->peekToken().is(Token::Kind::coloncolon)) {
        _lexer->getToken();
        module_path.push_back(identifier_name);
        tok = accept(Token::Kind::identifier);
        loc = tok.location;
        identifier_name = tok.value;
    }

    return std::make_unique<ast::IdentifierReference>(identifier_name,
                                                      module_path, loc);
}

std::vector<std::unique_ptr<ast::Expression>> Parser::parseArgs() {
    std::vector<std::unique_ptr<ast::Expression>> args;
    if (_lexer->peekToken().isNot(Token::Kind::r_paren)) {
        args.push_back(std::move(parseExpression()));
        while (_lexer->peekToken().is(Token::Kind::comma)) {
            _lexer->getToken();
            args.push_back(std::move(parseExpression()));
        }
    }
    return std::move(args);
}

void Parser::expect(Token::Kind kind) {
    while (!_lexer->peekToken().isOneOf(kind, Token::Kind::eof)) {
        auto tok = _lexer->getToken();
        _diag_engine->report(tok.location, 2001, tok.value);
    }
    _lexer->getToken();
}

Token Parser::accept(Token::Kind kind) {
    while (!_lexer->peekToken().isOneOf(kind, Token::Kind::eof)) {
        auto tok = _lexer->getToken();
        _diag_engine->report(tok.location, 2001, tok.value);
    }
    return _lexer->getToken();
}

} // namespace elang
