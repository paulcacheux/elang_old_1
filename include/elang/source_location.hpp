#ifndef ELANG_SOURCE_LOCATION_H
#define ELANG_SOURCE_LOCATION_H

#include <cstdlib>

namespace elang {

class SourceLocation {
  public:
    SourceLocation() = delete;
    SourceLocation(unsigned fileid, std::size_t offset);

    unsigned fileid;
    std::size_t offset;
};

} // namespace elang

#endif // ELANG_SOURCE_LOCATION_H
