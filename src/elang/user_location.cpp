#include <elang/user_location.hpp>

namespace elang {

UserLocation::UserLocation(std::string file_name, unsigned line,
                           unsigned column, std::string line_string,
                           bool is_eof)
    : file_name(file_name), line(line), column(column),
      line_string(line_string), is_eof(is_eof) {
}

} // namespace elang
