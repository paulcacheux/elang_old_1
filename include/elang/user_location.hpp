#ifndef ELANG_USER_LOCATION_H
#define ELANG_USER_LOCATION_H

#include <string>

namespace elang {

class UserLocation {
  public:
    std::string file_name;
    unsigned line;
    unsigned column;
    std::string line_string;
    bool is_eof;

    UserLocation(std::string file_name, unsigned line, unsigned column,
                 std::string line_string, bool is_eof);
};

} // namespace elang

#endif // ELANG_USER_LOCATION_H
