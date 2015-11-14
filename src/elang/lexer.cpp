#include <elang/lexer.hpp>

#include <iostream>
#include <cassert>

#include <elang/source_manager.hpp>
#include <elang/diagnostic.hpp>
#include <elang/source_location.hpp>

// utils
bool isSpace(int c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

bool isAlpha(int c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool isDigit(int c) {
    return '0' <= c && c <= '9';
}

namespace elang {

Lexer::Lexer(SourceManager* source_manager, unsigned fileid)
    : _source_manager(source_manager),
      _diag_engine(_source_manager->getDiagnosticEngine()),
      _reader(_source_manager->getBuffer(fileid)) {
}

Token Lexer::peekToken() {
    _waiting_tokens.push(getToken());
    return _waiting_tokens.top();
}

Token Lexer::getToken() {
    if (!_waiting_tokens.empty()) {
        auto tok = _waiting_tokens.top();
        _waiting_tokens.pop();
        return tok;
    }

    eatWhiteSpaces();
    auto token_location = _reader.getCurrentLocation();
    auto current = _reader.get();
    if (current == std::char_traits<char>::eof()) {
        return Token{Token::Kind::eof, token_location};
    } else if (current == '[') {
        return Token{Token::Kind::l_square, token_location};
    } else if (current == ']') {
        return Token{Token::Kind::r_square, token_location};
    } else if (current == '(') {
        return Token{Token::Kind::l_paren, token_location};
    } else if (current == ')') {
        return Token{Token::Kind::r_paren, token_location};
    } else if (current == '{') {
        return Token{Token::Kind::l_brace, token_location};
    } else if (current == '}') {
        return Token{Token::Kind::r_brace, token_location};
    } else if (current == '*') {
        return Token{Token::Kind::star, token_location};
    } else if (current == '+') {
        return Token{Token::Kind::plus, token_location};
    } else if (current == '%') {
        return Token{Token::Kind::percent, token_location};
    } else if (current == ';') {
        return Token{Token::Kind::semi, token_location};
    } else if (current == ',') {
        return Token{Token::Kind::comma, token_location};
    } else if (current == '.') {
        return Token{Token::Kind::dot, token_location};
    } else if (current == '/') {
        return Token{Token::Kind::slash, token_location};
    } else if (current == '-') {
        return Token{
            twoCharTokenKind('>', Token::Kind::minus, Token::Kind::arrow),
            token_location};
    } else if (current == '<') {
        return Token{
            twoCharTokenKind('=', Token::Kind::less, Token::Kind::lessequal),
            token_location};
    } else if (current == '>') {
        return Token{twoCharTokenKind('=', Token::Kind::greater,
                                      Token::Kind::greaterequal),
                     token_location};
    } else if (current == ':') {
        return Token{
            twoCharTokenKind(':', Token::Kind::colon, Token::Kind::coloncolon),
            token_location};
    } else if (current == '=') {
        return Token{
            twoCharTokenKind('=', Token::Kind::equal, Token::Kind::equalequal),
            token_location};
    } else if (current == '!') {
        return Token{twoCharTokenKind('=', Token::Kind::exclaim,
                                      Token::Kind::exclaimequal),
                     token_location};
    } else if (current == '&') {
        return Token{
            twoCharTokenKind('&', Token::Kind::amp, Token::Kind::ampamp),
            token_location};
    } else if (current == '|') {
        return Token{
            twoCharTokenKind('|', Token::Kind::pipe, Token::Kind::pipepipe),
            token_location};
    } else if (current == '\"') {
        std::string literal_string;
        while (_reader.peek() != '\"') {
            if (_reader.peek() == std::char_traits<char>::eof()) {
                _diag_engine->report(token_location, 1002);
            }
            literal_string.push_back(readLiteralChar());
        }
        _reader.get();
        return Token{Token::Kind::string_literal, token_location,
                     literal_string};
    } else if (current == '\'') {
        std::string literal_char;
        literal_char.push_back(readLiteralChar());
        if (_reader.get() != '\'') {
            _diag_engine->report(token_location, 1003);
        }
        return Token{Token::Kind::char_literal, token_location, literal_char};
    } else if (current == '#') {
        readComment();
        return getToken();
    } else if (isAlpha(current)) {
        _reader.unget();
        return makeIdentiferOrKeywordToken();
    } else if (isDigit(current)) {
        _reader.unget();
        return makeIntegerOrDoubleLiteralToken();
    } else {
        _diag_engine->report(token_location, 1001,
                             std::string{1, static_cast<char>(current)});
        return getToken();
    }
}

bool Lexer::matchChar(int expected) {
    if (_reader.peek() != expected)
        return false;
    _reader.get();
    return true;
}

Token::Kind Lexer::twoCharTokenKind(int expected, Token::Kind tk1,
                                    Token::Kind tk2) {
    return matchChar(expected) ? tk2 : tk1;
}

void Lexer::eatWhiteSpaces() {
    while (isSpace(_reader.peek())) {
        _reader.get();
    }
}

std::string Lexer::readIdentifierOrKeyword() {
    std::string buffer;
    while (isAlpha(_reader.peek()) || isDigit(_reader.peek())) {
        buffer.push_back(_reader.get());
    }
    return buffer;
}

std::string Lexer::readNumber() {
    std::string buffer;
    while (isDigit(_reader.peek())) {
        buffer.push_back(_reader.get());
    }
    return buffer;
}

char Lexer::readLiteralChar() {
    char c = _reader.get();
    if (c == '\\') {
        auto escape = _reader.get();
        switch (escape) {
        case 'a':
            c = '\a';
            break;
        case 'b':
            c = '\b';
            break;
        case 't':
            c = '\t';
            break;
        case 'n':
            c = '\n';
            break;
        case 'v':
            c = '\v';
            break;
        case 'f':
            c = '\f';
            break;
        case 'r':
            c = '\r';
            break;
        case '\"':
            c = '\"';
            break;
        case '\'':
            c = '\'';
            break;
        case '\?':
            c = '\?';
            break;
        case '\\':
            c = '\\';
            break;
        default:
            _diag_engine->report(_reader.getCurrentLocation(), 1004,
                                 std::string{1, static_cast<char>(escape)});
        }
    }
    return c;
}

void Lexer::readComment() {
    while (_reader.peek() != '\n'
           && _reader.peek() != std::char_traits<char>::eof()) {
        _reader.get();
    }
}

Token Lexer::makeIdentiferOrKeywordToken() {
    auto loc = _reader.getCurrentLocation();
    auto identifier_or_keyword = readIdentifierOrKeyword();
    return Token::fromIdentifier(loc, identifier_or_keyword);
}

Token Lexer::makeIntegerOrDoubleLiteralToken() {
    auto loc = _reader.getCurrentLocation();
    auto is_double = false;

    std::string value;
    value = readNumber();
    if (_reader.peek() == '.') {
        is_double = true;
        value.push_back(_reader.get());
        value += readNumber();
    }

    if (_reader.peek() == 'e' || _reader.peek() == 'E') {
        is_double = true;
        value.push_back(_reader.get());
        if (_reader.peek() == '+' || _reader.peek() == '-') {
            value.push_back(_reader.get());
            value += readNumber();
        }
    }

    return Token{is_double ? Token::Kind::double_literal
                           : Token::Kind::int_literal,
                 loc, value};
}

} // namespace elang
