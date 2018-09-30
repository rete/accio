
#ifndef ACCIO_BUFFER_H
#define ACCIO_BUFFER_H 1

// -- std headers
#include <memory>
#include <cstring>
#include <limits>
#include <iostream>
#include <type_traits>

// -- fio headers
#include <accio/definitions.h>
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
            class alloc = std::allocator<charT>>
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
    typedef types::ptr_type         ptr_type;
    typedef types::address_type     address_type;
    typedef std::size_t             pos_type;
    typedef long                    off_type;
    typedef std::size_t             size_type;
    typedef std::ios_base::seekdir  seek_dir;
    typedef std::ios_base::openmode open_mode;
    typedef std::ios_base::iostate  io_state;
    typedef types::pointed_at       pointed_at;
    typedef types::pointer_to       pointer_to;

    // constants
    static constexpr size_type      default_size = 2*1024*1024; // 2 Mo to start ...
    static constexpr size_type      default_expand = 1024*1024; // buffer expand size

  public:
    /// Allocate a buffer in write mode
    template <class = typename std::enable_if<sizeof(charT)==1,charT>::type>
    buffer(size_type size = default_size);

    /// Adopt/copy the buffer and set the buffer in read mode
    template <class = typename std::enable_if<sizeof(charT)==1,charT>::type>
    buffer(char_type *bytes, size_type size, bool cpy = false);
    
    /// Read n bytes from the file handle and set the buffer in read mode
    template <class = typename std::enable_if<sizeof(charT)==1,charT>::type>
    buffer(FILE *file, size_type size);

    /// Destructor. Always call delete on char buffer
    ~buffer();

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
    size_type bufcpy(char_type *data, size_type size);

    /// Seek the pointer in the buffer by an offset in the specifed way
    pos_type seekoff(off_type off, seek_dir way);

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
    size_type expand(size_type len);

    /// Read a bunch of data.
    /// Returns the size of actual read data
    /// Returns 0 if not in read mode
    size_type read(char_type *data, size_type memlen, size_type count);

    /// Write a bunch of data.
    /// Returns the size of actual written data
    /// Returns 0 if not in write mode
    size_type write(const char_type *data, size_type memlen, size_type count);

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
    
    /// Write an address
    size_type write_pointer(const address_type *addr);
    
    /// Read an address 'pointed at'
    size_type read_pointed_at(address_type *addr);
    
    /// Read an address 'pointer to'
    size_type read_pointer_to(address_type **addr);

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
    
    /// Relocate the pointers in memory after a read operation
    /// This will also clear the internal maps of so called
    /// 'pointer to' and 'pointed at'
    bool relocate();

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
    /// The map of pointers 'pointed at'
    pointed_at                 m_pointed_at{};
    /// The map of pointers 'pointer to'
    pointer_to                 m_pointer_to{};
  };
}

#include <accio/details/buffer_impl.h>

#endif  //  ACCIO_BUFFER_H
