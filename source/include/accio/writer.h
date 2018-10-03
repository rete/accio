

#ifndef ACCIO_WRITER_H
#define ACCIO_WRITER_H 1

#include <accio/definitions.h>
#include <accio/stream.h>

namespace accio {

  // forward declaration
  template <class charT, class copy, class alloc>
  class buffer;

  /// block_writer class
  ///
  /// Main interface for writing a block in a record
  template <typename config>
  class block_writer {
  public:
    typedef typename config::char_type                                     char_type;
    typedef typename config::copy_type                                     copy_type;
    typedef typename config::allocator_type                                allocator_type;
    typedef typename accio::buffer<char_type, copy_type, allocator_type>   buffer_type;
    typedef string64                                                       string_type;
    typedef types::version_type                                            version_type;

  public:
    /// Constructor with block version
    block_writer(
      const string_type &t,
      const string_type &n,
      version_type vers) :
      m_version(vers),
      m_type(t),
      m_name(n) {
      /* nop */
    }

    /// Get the block version
    inline version_type version() const {
      return m_version;
    }

    /// Get the block type
    inline const string_type &type() const {
      return m_type;
    }

    /// Get the block name
    inline const string_type &name() const {
      return m_name;
    }

    /// Write a block in a buffer.
    virtual error_codes::code_type write(buffer_type &outbuf) const = 0;

  private:
    /// The block version
    const version_type                   m_version;
    /// The block type
    const string_type                    m_type;
    /// The block name
    const string_type                    m_name;
  };

  template <typename config>
  class record_io {
  public:
    typedef typename config::record_type                   record_type;
    typedef typename config::char_type                     char_type;
    typedef typename config::copy_type                     copy_type;
    typedef typename accio::block_writer<config>           block_writer;
    typedef typename std::shared_ptr<const block_writer>   block_writer_ptr;
    typedef typename std::vector<block_writer_ptr>         block_writers;

  public:
    /// Method called when a record is written by a file writer
    /// The record object (user data) can be used to create the block writers
    virtual error_codes::code_type create_writers(const record_type& record, block_writers &blocks) const = 0;
  };

  template <class config>
  class file_writer {
  public:
    typedef typename config::char_type                 char_type;
    typedef typename config::copy_type                 copy_type;
    typedef typename config::record_type               record_type;
    typedef typename accio::record_io<config>          record_io;
    typedef typename record_io::block_writers          block_writers;

  public:
    /// Constructor
    file_writer() = default;
    /// Destructor
    ~file_writer() = default;

  public:
    /// Open a file in write mode
    error_codes::code_type open(const std::string &fname);

    // write a record
    error_codes::code_type write_record(
      const string32 &name,                // the record name to write
      const record_io &io_config,          // the io config: record and block settings
      const record_type &rec               // the record product to write: event, run header, etc ...
    );

  private:
    /// The record stream object
    typename accio::stream<char_type, copy_type>                 m_stream{};
  };
}

#include <accio/details/writer_impl.h>

#endif  //  ACCIO_WRITER_H
