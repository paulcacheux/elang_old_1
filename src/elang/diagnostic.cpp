#include <elang/diagnostic.hpp>

#include <iostream>
#include <ostream>

#include <elang/source_manager.hpp>

namespace elang {

std::ostream& operator<<(std::ostream& out, ErrorLevel level) {
    switch (level) {
    case ErrorLevel::Warning:
        return out << "warning";
    case ErrorLevel::FatalError:
        return out << "fatal error";
    case ErrorLevel::Error:
        return out << "error";
    }
}

DiagnosticEngine::DiagnosticEngine(SourceManager* sm, unsigned limit)
    : _source_manager(sm), _limit(limit), _nerr(0) {
}

void DiagnosticEngine::report(ErrorLevel level, SourceLocation loc,
                              std::string message) {
    auto user_loc = _source_manager->getUserLocation(loc);
    std::cout << user_loc.file_name << ":" << user_loc.line << ":"
              << user_loc.column << ": " << red_color << level << normal_color
              << " " << message << "\n";
    std::cout << user_loc.line_string << "\n";
    std::cout << std::string(user_loc.column, ' ') << "^\n";

    ++_nerr;
    if (level == ErrorLevel::FatalError || user_loc.is_eof) {
        std::exit(1);
    }
    if (_nerr >= _limit) {
        std::cout << _limit << ": compilation aborted" << std::endl;
        std::exit(1);
    }
}

void DiagnosticEngine::report(SourceLocation loc, std::string message) {
    report(ErrorLevel::Error, loc, message);
}

} // namespace elang
