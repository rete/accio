
#ifndef ACCIO_BUFFER_H
#define ACCIO_BUFFER_H 1

// -- std headers
#include <memory>
#include <cstring>
#include <limits>
#include <iostream>
#include <type_traits>

// -- fio headers
#include <accio/copy.h>

namespace accio {

  /// buffer class
  ///
  /// Main interface to write/read to/from a memory buffer.
  /// The copy template argument allows for chosing a conversion
  /// in a specific endianess. Default value is copy::standard,
  /// meaning that the byte ordering is conserved. To target
  /// big endian, use copy::big_endian and copy::little_endian
  /// for little endian copy.
  template <class charT,
            class copy = copy::standard,
            class alloc = std::allocator<charT>,
            class = typename std::enable_if<sizeof(charT)==1,charT>::type>
  class buffer {
    buffer() = delete;
    buffer(const buffer&) = delete;
    buffer& operator=(const buffer&) = delete;

  public:
    // traits
    typedef charT                   char_type;
    typedef alloc                   allocator_type;
    typedef copy                    copy_type;
    typedef int                     int_type;
    typedef std::size_t             pos_type;
    typedef long                    off_type;
    typedef std::size_t             size_type;
    typedef std::ios_base::seekdir  seek_dir;
    typedef std::ios_base::openmode open_mode;
    typedef std::ios_base::iostate  io_state;

    // constants
    static constexpr size_type      default_size = 2*1024*1024; // 2 Mo to start ...
    static constexpr size_type      default_expand = 1024*1024; // buffer expand size

  public:
    /// Allocate a buffer in write mode
    inline buffer(size_type size = default_size) {
      allocator_type allocator;
      m_buffer = allocator.allocate(size);
      m_size = size;
      m_memsize = size;
      std::memset(m_buffer, 0, m_size);
      m_current = m_buffer;
      m_mode = std::ios_base::out | std::ios_base::binary;
    }

    /// Adopt/copy the buffer and set the buffer in read mode
    inline buffer(char_type *bytes, size_type size, bool cpy = false) {
      if(nullptr == bytes) {
        setstate(std::ios_base::badbit);
        return;
      }
      if(cpy) {
        allocator_type allocator;
        m_buffer = allocator.allocate(size);
        std::memcpy(m_buffer, bytes, size);
      }
      else {
        m_buffer = bytes;
      }
      m_current = m_buffer;
      m_size = size;
      m_memsize = size;
      m_mode = std::ios_base::in | std::ios_base::binary;
    }

    /// Destructor. Always call delete on char buffer
    inline ~buffer() {
      if(nullptr != m_buffer) {
        delete [] m_buffer;
      }
      m_buffer = nullptr;
      m_current = nullptr;
    }

    /// Get the buffer size
    inline size_type size() const noexcept {
      return m_size;
    }

    /// Get the buffer memory size
    inline size_type memsize() const noexcept {
      return m_memsize;
    }

    /// Get the distance between the current buffer position
    /// and the end of the buffer
    inline size_type remaining() const noexcept {
      return (size() - tell());
    }

    /// Copy the input buffer. Returns the number of copied len
    /// Reset the position of the buffer at beginning
    inline size_type bufcpy(char_type *data, size_type size) {
      if(nullptr == data) {
        setstate(std::ios_base::badbit);
        return 0;
      }
      // save a memory allocation if buffer size if
      // buffer size in memory is bigger
      if(size > m_memsize) {
        if(nullptr != m_buffer) {
          delete [] m_buffer;
          m_buffer = nullptr;
        }
        allocator_type allocator;
        m_buffer = allocator.allocate(size);
      }
      std::memcpy(m_buffer, data, size);
      m_size = size;
      m_current = m_buffer;
      clear_state();
      return m_size;
    }

    /// Seek the pointer in the buffer by an offset in the specifed way
    inline pos_type seekoff(off_type off, seek_dir way) {
      // from beginning
      if(std::ios_base::beg == way) {
        if(off > m_size) {
          setstate(std::ios_base::failbit);
        }
        else {
          m_current = begin() + off;
        }
      }
      // from end of buffer
      else if(std::ios_base::end == way) {
        if(off > m_size) {
          setstate(std::ios_base::failbit);
        }
        else {
          m_current = end() - off;
        }
      }
      // from current position
      else {
        if(off > (end() - current())) {
          setstate(std::ios_base::failbit);
        }
        else {
          m_current = current() + off;
        }
      }
      return tell();
    }

    /// Seek the position to an absolute value
    inline pos_type seekpos(pos_type pos) {
      return seekoff(pos, std::ios_base::beg);
    }

    /// Get the distance from beginning of buffer
    inline pos_type tell() const {
      return static_cast<pos_type>(current() - begin());
    }

    /// Expand the buffer by adding 'len' char at the end of buffer
    /// Returns the number of char added
    inline size_type expand(size_type len) {
      if(0 == len) {
        return 0;
      }
      allocator_type allocator;
      size_type newlen = m_size + len;
      char_type* bytes = allocator.allocate(newlen);
      if(nullptr != m_buffer) {
        std::memcpy(bytes, m_buffer, m_size);
        std::memset(bytes + m_size, 0, len);
        delete [] m_buffer;
      }
      else {
        std::memset(bytes, 0, newlen);
      }
      m_buffer = bytes;
      clear_state();
      return len;
    }

    /// Read a bunch of data.
    /// Returns the size of actual read data
    /// Returns 0 if not in read mode
    inline size_type read(char_type *data, size_type memlen, size_type count) {
      if((nullptr == data) or (0 == memlen) or (0 == count)) {
        return 0;
      }
      if(nullptr == m_buffer) {
        setstate(std::ios_base::badbit);
        return 0;
      }
      // check read mode
      if(not (mode() & std::ios_base::in)) {
        setstate(std::ios_base::failbit);
        return 0;
      }
      // check remaining size
      auto rem = remaining();
      auto total = memlen*count;
      // reach end of buffer ?
      if(total > rem) {
        total = rem;
        setstate(std::ios_base::eofbit);
      }
      copy_type::memcpy(data, m_current, memlen, count);
      m_current += total;
      return total;
    }

    /// Write a bunch of data.
    /// Returns the size of actual written data
    /// Returns 0 if not in write mode
    inline size_type write(const char_type *data, size_type memlen, size_type count) {
      if((nullptr == data) or (0 == memlen) or (0 == count)) {
        return 0;
      }
      if(nullptr == m_buffer) {
        setstate(std::ios_base::badbit);
        return 0;
      }
      // check write mode
      if(not (mode() & std::ios_base::out)) {
        setstate(std::ios_base::failbit);
        return 0;
      }
      // expand the buffer if not enough space
      auto rem = remaining();
      auto total = memlen*count;
      if(total > rem) {
        auto expfac = ((total / rem) + 1)*default_expand;
        if(expfac != expand(expfac)) {
          setstate(std::ios_base::failbit);
          return 0;
        }
      }
      copy_type::memcpy(m_current, data, memlen, count);
      m_current += total;
      return total;
    }

    /// Write a bunch of data, either a single value or array
    template <typename T>
    inline size_type write_data(const T &data, size_type len = 1) noexcept {
      return write(reinterpret_cast<const char_type*>(&data), sizeof(T), len);
    }

    /// Read a bunch of data, either a single value or array
    template <typename T>
    inline size_type read_data(T &data, size_type len = 1) noexcept {
      return read(reinterpret_cast<char_type*>(&data), sizeof(T), len);
    }

    /// Get the opening mode
    inline open_mode mode() const noexcept {
      return m_mode;
    }

    /// Get position at the begining of the buffer
    inline const char_type* begin() const noexcept {
      return m_buffer;
    }

    /// Get the current ptr position
    inline const char_type* current() const noexcept {
      return m_current;
    }

    /// Get the pointer at end of the buffer
    /// WARNING: this pointer can be changed in write mode
    /// if the buffer is expanded
    inline const char_type* end() const noexcept {
      return ((nullptr == m_buffer) ? nullptr : (m_buffer + m_size));
    }

    /// non-const version of begin
    inline char_type* begin() noexcept {
      return m_buffer;
    }

    /// non-const version of current
    inline char_type* current() noexcept {
      return m_current;
    }

    /// non-const version of end
    inline char_type* end() noexcept {
      return ((nullptr == m_buffer) ? nullptr : (m_buffer + m_size));
    }

    /// Returns the buffer state
    inline io_state rdstate() const noexcept {
      return m_iostate;
    }

    /// Set the buffer state
    inline void setstate(io_state state) noexcept {
      clear_state(m_iostate | state);
    }

    /// Whether the buffer is in a good state
    inline bool good() const noexcept {
      return (m_iostate == std::ios_base::goodbit);
    }

    /// Whether we have reach the end of buffer
    /// Only possible in read mode as in write mode
    /// the buffer is expanded and thus we never reach it
    inline bool eof() const noexcept {
      return ((m_iostate & std::ios_base::eofbit) != 0);
    }

    /// Whether the buffer is in fail state
    inline bool fail() const noexcept {
      return ((m_iostate & (std::ios_base::failbit | std::ios_base::badbit)) != 0);
    }

    /// Whether the buffer is in bad state
    inline bool bad() const noexcept {
      return ((m_iostate & std::ios_base::badbit) != 0);
    }

    /// Clear the buffer state
    inline void clear_state(io_state state = std::ios_base::goodbit) noexcept {
      m_iostate = state;
    }

  private:
    /// The buffer open mode
    open_mode                  m_mode{std::ios_base::out};
    /// The buffer io state
    io_state                   m_iostate{std::ios_base::goodbit};
    /// The buffer size
    size_type                  m_size{0};
    /// The buffer size in memory
    size_type                  m_memsize{0};
    /// The actual raw buffer
    char_type*                 m_buffer{nullptr};
    /// The current read/write position in the buffer
    char_type*                 m_current{nullptr};
  };

}

#endif  //  ACCIO_BUFFER_H
