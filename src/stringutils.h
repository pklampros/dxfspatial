#pragma once

namespace stringutils {

bool startsWith(const std::string & str, const std::string & prefix) {
  return str.substr(0, prefix.length()) == prefix;
}

bool endsWith(const std::string & str, const std::string & suffix) {
  return str.substr(str.length() - suffix.length(), str.length()) == suffix;
}

std::string& toLower(std::string &str)
{
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

// trim from start (in place)
void ltrim(std::string &s, char c) {
  // if getting errors here when compiling make sure that c++11 is used
  // i.e. put PKG_CXXFLAGS = -std=c++11 in the ~/.R/Makevars file
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&c](int ch) {
    return ch != c;
  }));
}

// trim from end (in place)
void rtrim(std::string &s, char c) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [&c](int ch) {
    return ch != c;
  }).base(), s.end());
}

std::istream& safeGetline(std::istream& is, std::string& t)
{
  t.clear();
  // https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.

  std::istream::sentry se(is, true);
  std::streambuf* sb = is.rdbuf();

  for(;;) {
    int c = sb->sbumpc();
    switch (c) {
    case '\n':
      return is;
    case '\r':
      if(sb->sgetc() == '\n')
        sb->sbumpc();
      return is;
    case std::streambuf::traits_type::eof():
      // Also handle the case when the last line has no line ending
      if(t.empty())
        is.setstate(std::ios::eofbit);
      return is;
    default:
      t += (char)c;
    }
  }
}
}
