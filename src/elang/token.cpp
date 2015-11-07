#include <elang/token.hpp>

namespace elang {

Token Token::fromIdentifier(SourceLocation loc, const std::string& id) {
    if (id == "true" || id == "false") {
        return Token{Token::Kind::boolean_literal, loc, id};
    }

#define KEYWORD(X)                                                             \
    if (id == #X) {                                                            \
        return Token{Token::Kind::kw_##X, loc};                                \
    }
#include <elang/tokenkind.def>

    else {
        return Token{Token::Kind::identifier, loc, id};
    }
}

} // namespace elang

std::ostream& operator<<(std::ostream& out, const elang::Token::Kind& kind) {
    switch (kind) {
#define TOK(X)                                                                 \
    case elang::Token::Kind::X:                                                \
        out << #X;                                                             \
        break;
#include <elang/tokenkind.def>
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const elang::Token& tok) {
    // return out << "(" << tok.getSourceLocation() << ") [" << tok.getKind()
    //           << "] " << tok.getValue();
    return out << "[" << tok.kind << "]" << tok.value;
}
