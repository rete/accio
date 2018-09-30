
#ifndef ACCIO_STREAM_H
#define ACCIO_STREAM_H 1

// -- fio headers
#include <accio/definitions.h>
#include <accio/copy.h>
#include <accio/buffer.h>

namespace accio {

  /// stream class.
  ///
  /// Main interface to file stream (read or write)
  template <class charT, class copy = copy::standard>
  class stream {
  public:
    typedef charT                              char_type;
    typedef copy                               copy_type;
    typedef buffer<char_type, copy_type>       buffer_type;
    typedef typename buffer_type::size_type    size_type;
    typedef FILE                               file_type;

  public:
    stream() = default;
    stream(const stream&) = delete;
    stream &operator=(const stream&) = delete;
    ~stream() = default;

    /// Get the file name
    inline const std::string& fname() const noexcept {
      return m_fname;
    }

    /// Get the open state (opened, closed, error)
    inline io::open_state open_state() const noexcept {
      return m_openstate;
    }

    /// Get the open mode
    inline io::open_mode open_mode() const noexcept {
      return m_openmode;
    }

    /// open a file
    error_codes::code_type open(const std::string& fn, io::open_mode mode) noexcept;

    /// close the file
    error_codes::code_type close() noexcept;
    
    error_codes::code_type write_record(
      const io::record_header &header,
      const io::record_summary &summary,
      const buffer_type &buffer
    );

  private:
    /// The stream open mode
    io::open_mode              m_openmode{io::open_mode::read};
    /// The current opened file name
    std::string                m_fname{};
    /// The stream open state
    io::open_state             m_openstate{io::open_state::closed};
    /// The file handle
    file_type*                 m_file{nullptr};
  };
}

#include <accio/details/stream_impl.h>

#endif  //  ACCIO_STREAM_H
