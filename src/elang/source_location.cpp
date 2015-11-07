#include <elang/source_location.hpp>

namespace elang {

SourceLocation::SourceLocation(unsigned fileid, std::size_t offset)
    : fileid(fileid), offset(offset) {
}

} // namespace elang
