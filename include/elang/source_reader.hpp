#ifndef ELANG_SOURCE_READER_H
#define ELANG_SOURCE_READER_H

#include <string>

#include <elang/source_location.hpp>

namespace elang {

class SourceReader {
    std::string::const_iterator _begin;
    std::string::const_iterator _current;
    std::string::const_iterator _end;
    SourceLocation _current_location;

  public:
    SourceReader(std::string::const_iterator begin,
                 std::string::const_iterator end, unsigned fileid)
        : _begin(begin), _current(begin), _end(end),
          _current_location(fileid, 0) {
    }

    int get() {
        if (_current == _end) {
            return std::char_traits<char>::eof();
        }
        ++_current_location.offset;
        return *(_current++);
    }

    int peek() {
        if (_current == _end) {
            return std::char_traits<char>::eof();
        }
        return *_current;
    }

    void unget() {
        --_current;
        --_current_location.offset;
    }

    SourceLocation getCurrentLocation() {
        return _current_location;
    }
};

} // namespace elang

#endif // ELANG_SOURCE_READER_H
