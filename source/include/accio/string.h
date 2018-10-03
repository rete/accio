//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#ifndef ACCIO_STRING_H
#define ACCIO_STRING_H 1

// -- std headers
#include <cstring>
#include <string>
#include <string.h>

namespace accio {

  /// A fixed size string class version
  ///
  /// Note that most of the methods mimic the std::string interface.
  /// The purpose of this class is:
  ///   - to have a fixed sizeof(str) value for a particular string
  ///   - to allow their use in PODs
  template <std::size_t len>
  class string {
  private:
    /// The C string fixed-size array
    char              m_string[len];

  public:
    /// Constructor
    inline string() {
      std::memset(m_string, '\0', len);
    }

    /// Copy constructor
    inline string(const accio::string<len> &str) {
      std::memcpy(m_string, str, len);
    }

    /// Constructor with C-string
    inline string(const char *str) {
      assign(str, strlen(str));
    }

    /// Constructor with C-string and size
    inline string(const char *str, std::size_t ilen) {
      assign(str, ilen);
    }

    /// Constructor with std::string
    inline string(const std::string &str) {
      assign(str.c_str(), str.size());
    }

    /// The actual string len (strlen)
    inline std::size_t size() const {
      return strlen(m_string);
    }

    /// The total array capacity (template parameter len)
    inline std::size_t capacity() const {
      return len;
    }

    /// The maximum usable string size. As the string must be null
    /// terminated, the last char is reserved for '\0' (len-1)
    inline std::size_t max_size() const {
      return len-1;
    }

    /// Assign a new string to this
    /// Returns the number of truncated bytes if the size of
    /// the input string if greater than the len of this string
    inline std::size_t assign(const char *str, std::size_t ilen) {
      std::memset(m_string, '\0', len);
      auto actual_len = std::min(len-1, ilen);
      std::memcpy(m_string, str, actual_len);
      return (ilen > len-1) ? (ilen - (len-1)) : 0;
    }

    /// C-string access
    inline const char *c_str() const {
      return &m_string[0];
    }

    /// Construct an std::string from this string
    inline std::string std() const {
      return std::string(c_str());
    }

    /// Operator 'at'
    inline char &operator[](std::size_t pos) {
      return m_string[pos];
    }

    /// Operator 'at' (const version)
    inline const char &operator[](std::size_t pos) const {
      return m_string[pos];
    }

    /// Assignment operator. Same len only
    inline string<len> &operator=(const string<len> &str) {
      std::memcpy(m_string, str.m_string, len);
    }

    /// Assignment operator. Same len only
    inline friend bool operator==(const string<len> &lhs, const char *rhs) {
      return (strcmp(&lhs.m_string[0], rhs) == 0);
    }

    /// Get a character at a specific position.
    /// Throw out_of_range exception if not in string range
    inline char &at(std::size_t pos) {
      if(pos >= size()) {
        throw std::out_of_range("accio::string::at: out of range");
      }
      return m_string[pos];
    }

    /// Get a character at a specific position (const version)
    inline const char &at(std::size_t pos) const {
      if(pos >= size()) {
        throw std::out_of_range("accio::string::at: out of range");
      }
      return m_string[pos];
    }
  };

  /// specialization for 0 and 1 len string (disable)
  /// They just make no sense ...
  template <> class string<0> {};
  template <> class string<1> {};

  // useful 32 bit padded string typedefs
  typedef string<4>          string4;
  typedef string<8>          string8;
  typedef string<16>         string16;
  typedef string<32>         string32;
  typedef string<64>         string64;
  typedef string<128>        string128;
  typedef string<256>        string256;

}

#endif  //  ACCIO_STRING_H
