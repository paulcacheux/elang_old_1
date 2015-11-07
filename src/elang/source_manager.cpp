#include <elang/source_manager.hpp>

#include <iostream>
#include <fstream>
#include <iterator>
#include <memory>

namespace elang {

util::FileRecord::FileRecord(std::string file_path, std::string buffer)
    : file_path(file_path), buffer(buffer) {
}

SourceManager::SourceManager() : _diag_engine(this, 5) {
}

unsigned SourceManager::registerFile(std::string file_path) {
    std::ifstream input_file{file_path};
    if (!input_file.is_open()) {
        throw std::runtime_error("Can't read from" + file_path);
    }

    std::string buffer{std::istreambuf_iterator<char>{input_file},
                       std::istreambuf_iterator<char>{}};

    _records.emplace_back(file_path, buffer);
    return _records.size() - 1;
}

unsigned SourceManager::registerStdin() {
    std::string buffer{std::istreambuf_iterator<char>{std::cin},
                       std::istreambuf_iterator<char>{}};
    _records.emplace_back("<stdin>", buffer);
    return _records.size() - 1;
}

SourceReader SourceManager::getBuffer(unsigned fileid) {
    auto& s = _records[fileid].buffer;
    return SourceReader{s.cbegin(), s.cend(), fileid};
}

DiagnosticEngine* SourceManager::getDiagnosticEngine() {
    return &_diag_engine;
}

TypeManager* SourceManager::getTypeManager() {
    return &_type_manager;
}

UserLocation SourceManager::getUserLocation(const SourceLocation& loc) {
    const auto& buffer = _records[loc.fileid].buffer;
    auto begin_pos = buffer.find_last_of('\n', loc.offset);

    if (begin_pos == std::string::npos) {
        begin_pos = 0;
    } else {
        ++begin_pos;
    }

    auto end_pos = buffer.find_first_of('\n', loc.offset);
    auto line_str = buffer.substr(begin_pos, end_pos - begin_pos);

    unsigned line
        = std::count(buffer.begin(), buffer.begin() + loc.offset, '\n') + 1;
    unsigned column = loc.offset - begin_pos;

    bool is_eof = false;
    if (loc.offset >= buffer.size())
        is_eof = true;

    return UserLocation{_records[loc.fileid].file_path, line, column, line_str,
                        is_eof};
}

} // namespace elang
