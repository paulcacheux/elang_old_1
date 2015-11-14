#include <elang/diagnostic.hpp>

#include <iostream>
#include <ostream>
#include <cassert>

#include <elang/source_manager.hpp>

std::string getMessage(unsigned error_index);
std::vector<std::string> splitMessage(std::string msg);
std::string buildMessage(unsigned error_index,
                         std::initializer_list<std::string> params);

namespace elang {

DiagnosticEngine::DiagnosticEngine(SourceManager* sm, unsigned limit)
    : _source_manager(sm), _limit(limit), _nerr(0) {
}

void DiagnosticEngine::report(SourceLocation loc, unsigned error_index,
                              std::initializer_list<std::string> params) {
    auto message = buildMessage(error_index, params);
    auto user_loc = _source_manager->getUserLocation(loc);
    std::cout << user_loc.file_name << ":" << user_loc.line << ":"
              << user_loc.column << ": " << red_color
              << "Error :" << normal_color << " " << message << "\n";
    std::cout << user_loc.line_string << "\n";
    std::cout << std::string(user_loc.column, ' ') << "^\n";

    ++_nerr;
    if (user_loc.is_eof) {
        std::exit(1);
    }
    if (_nerr >= _limit) {
        std::cout << _limit << " errors : compilation aborted" << std::endl;
        std::exit(1);
    }
}

} // namespace elang

std::string getMessage(unsigned error_index) {
    switch (error_index) {
#define MSG(index, msg)                                                        \
    case index:                                                                \
        return msg;
#include <elang/error_messages.def>
    default:
        return getMessage(0);
    }
}

std::vector<std::string> splitMessage(std::string msg) {
    std::vector<std::string> parts;
    std::string current;
    for (auto& c : msg) {
        if (c == '@') {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    return parts;
}

std::string buildMessage(unsigned error_index,
                         std::initializer_list<std::string> params) {
    auto template_message = getMessage(error_index);
    auto split_msg = splitMessage(template_message);
    assert(split_msg.size() - 1 == params.size());

    auto it = split_msg.begin();
    std::string final_message = *(it++);
    for (auto& param : params) {
        final_message += param + *(it++);
    }
    return final_message;
}
