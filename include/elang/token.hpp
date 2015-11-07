#ifndef ELANG_TOKEN_H
#define ELANG_TOKEN_H

#include <string>
#include <ostream>

#include <elang/source_location.hpp>

namespace elang {

class Token {
  public:
    enum class Kind {
#define TOK(X) X,
#include <elang/tokenkind.def>
    };

    Kind kind;
    SourceLocation location;
    std::string value;

    Token() = delete;
    Token(Kind kind, SourceLocation loc, std::string value = {})
        : kind(kind), location(loc), value(value) {
    }

    static Token fromIdentifier(SourceLocation loc, const std::string& id);

    bool is(Kind k) const noexcept {
        return kind == k;
    }

    bool isNot(Kind k) const noexcept {
        return kind != k;
    }

    bool isOneOf(Kind k1, Token::Kind k2) const noexcept {
        return is(k1) || is(k2);
    }

    template <class... Ts>
    bool isOneOf(Kind k1, Kind k2, Ts... ks) const noexcept {
        return is(k1) || isOneOf(k2, ks...);
    }
};

Token::Kind nameKind(const std::string& id);

} // namespace elang

std::ostream& operator<<(std::ostream& out, const elang::Token::Kind& kind);
std::ostream& operator<<(std::ostream& out, const elang::Token& tok);

#endif // CMINUS_TOKEN_H
