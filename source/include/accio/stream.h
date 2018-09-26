
#ifndef ACCIO_STREAM_H
#define ACCIO_STREAM_H 1

// -- fio headers
#include <accio/definitions.h>
#include <accio/copy.h>
#include <accio/buffer.h>

namespace accio {

  struct record_info {
    types::option_word        m_options{0};
    types::size_type          m_length{0};
    types::size_type          m_clength{0};
    std::string               m_name{};
  };

  /// stream class.
  ///
  /// Main interface to file stream (read or write)
  class stream {
  public:
    typedef copy::big_endian                   copy_type;
    typedef buffer<unsigned char, copy_type>   stream_buffer;
    typedef FILE                               file_type;

  public:
    stream() = delete;
    stream(const stream&) = delete;
    stream &operator=(const stream&) = delete;
    ~stream() = default;

    /// Constructor with stream name
    inline stream(const std::string &inname) :
      m_name(inname) {
      /* nop */
    }

    /// Get the name of stream
    inline const std::string& name() const noexcept {
      return m_name;
    }

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

    ///
    error_codes::code_type read_next_record_info(record_info &info);

  private:
    
    error_codes::code_type read_next_record_info(record_info &info, bool reset);

  private:
    /// The stream open mode
    io::open_mode              m_openmode{io::open_mode::read};
    /// The stream name
    std::string                m_name{};
    /// The current opened file name
    std::string                m_fname{};
    /// The stream open state
    io::open_state             m_openstate{io::open_state::closed};
    /// The file handle
    file_type*                 m_file{nullptr};
  };
}

#endif  //  ACCIO_STREAM_H
