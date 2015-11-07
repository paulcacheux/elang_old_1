#include <elang/diagnoctic.hpp>

#include <iostream>
#include <ostream>
#include <string>

#include <elang/source_manager.hpp>

namespace elang {

const std::string red_color{"\033[31m"};
const std::string normal_color{"\033[0m"};

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
    ++_nerr;
    auto user_loc = _source_manager->getUserLocation(loc);
    std::cout << user_loc.file_name << ":" << user_loc.line << ":"
              << user_loc.column << ": " << level << " " << red_color << message
              << normal_color << "\n";
    std::cout << user_loc.line_string << "\n";
    std::cout << std::string(user_loc.column, ' ') << "^\n";

    if (level == ErrorLevel::FatalError || user_loc.is_eof) {
        std::exit(1);
    }
    if (_nerr >= _limit) {
        std::cout << _limit << ": compilation aborted" << std::endl;
        std::exit(1);
    }
}

} // namespace elang
