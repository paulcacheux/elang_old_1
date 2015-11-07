#ifndef ELANG_DIAGNOSTIC_H
#define ELANG_DIAGNOSTIC_H

#include <string>

#include <elang/source_location.hpp>

namespace elang {

enum class ErrorLevel { Warning, FatalError, Error };

class SourceManager;

class DiagnosticEngine {
    SourceManager* _source_manager;
    unsigned _limit;
    unsigned _nerr;

  public:
    DiagnosticEngine(SourceManager* sm, unsigned limit = 5);
    void report(ErrorLevel level, SourceLocation loc, std::string message);
};

} // namespace elang

#endif // ELANG_DIAGNOSTIC_H
