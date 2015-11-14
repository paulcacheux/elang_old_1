#ifndef ELANG_DIAGNOSTIC_H
#define ELANG_DIAGNOSTIC_H

#include <string>
#include <vector>

#include <elang/source_location.hpp>

namespace elang {

class SourceManager;

class DiagnosticEngine {
    SourceManager* _source_manager;
    unsigned _limit;
    unsigned _nerr;
    const std::string red_color{"\033[31m"};
    const std::string normal_color{"\033[0m"};

  public:
    DiagnosticEngine(SourceManager* sm, unsigned limit = 5);

    template <class... Ts>
    void report(SourceLocation loc, unsigned error_index, Ts... params) {
        return report(loc, error_index, {params...});
    }

  private:
    void report(SourceLocation loc, unsigned error_index,
                std::initializer_list<std::string> params);
};

} // namespace elang

#endif // ELANG_DIAGNOSTIC_H
