#ifndef ELANG_SOURCE_MANAGER_H
#define ELANG_SOURCE_MANAGER_H

#include <vector>
#include <string>

#include <elang/type.hpp>
#include <elang/diagnoctic.hpp>
#include <elang/source_reader.hpp>
#include <elang/user_location.hpp>

namespace elang {

namespace util {
struct FileRecord {
    FileRecord(std::string file_path, std::string buffer);
    std::string file_path;
    std::string buffer;
};
}

class SourceManager {
    std::vector<util::FileRecord> _records;
    DiagnosticEngine _diag_engine;
    TypeManager _type_manager;

  public:
    SourceManager();
    SourceManager(const SourceManager&) = delete;
    SourceManager(SourceManager&&) = delete;
    SourceManager& operator=(const SourceManager&) = delete;
    SourceManager& operator=(SourceManager&&) = delete;

    unsigned registerFile(std::string file_path);
    unsigned registerStdin();
    SourceReader getBuffer(unsigned fileid);
    DiagnosticEngine* getDiagnosticEngine();
    TypeManager* getTypeManager();
    UserLocation getUserLocation(const SourceLocation& loc);
};

} // namespace elang

#endif // ELANG_SOURCE_MANAGER_H
