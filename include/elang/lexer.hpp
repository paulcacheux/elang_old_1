#ifndef ELANG_LEXER_H
#define ELANG_LEXER_H

#include <string>
#include <stack>

#include <elang/source_reader.hpp>
#include <elang/token.hpp>

namespace elang {

class SourceManager;
class DiagnosticEngine;

class Lexer {
    SourceManager* _source_manager;
    DiagnosticEngine* _diag_engine;
    SourceReader _reader;
    std::stack<Token> _waiting_tokens;

  public:
    explicit Lexer(SourceManager* source_manager, unsigned fileid);
    Token peekToken();
    Token getToken();

  private:
    bool matchChar(int expected);
    Token::Kind twoCharTokenKind(int expected, Token::Kind tk1,
                                 Token::Kind tk2);

    void eatWhiteSpaces();
    std::string readIdentifierOrKeyword();
    std::string readNumber();
    char readLiteralChar();
    void readComment();
    Token makeIdentiferOrKeywordToken();
    Token makeIntegerOrDoubleLiteralToken();
};

} // namespace elang

#endif // ELANG_LEXER_H
