//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#ifndef ACCIO_STREAM_IMPL_H
#define ACCIO_STREAM_IMPL_H 1

#include <vector>

namespace accio {

  // open a file
  template <class charT, class copy>
  inline error_codes::code_type stream<charT, copy>::open(const std::string& fn, io::open_mode mode) noexcept {
    if((io::open_state::opened == m_openstate) or (io::open_state::error == m_openstate)) {
      return error_codes::stream::already_open;
    }
    std::string mode_str = io::open_mode_str(mode);
    m_file = io::file::open(fn.c_str(), mode_str.c_str());
    if(nullptr == m_file) {
      return error_codes::stream::open_fail;
    }
    m_fname = fn;
    m_openmode = mode;
    m_openstate = io::open_state::opened;
    // That's all folks!
    return error_codes::stream::success;
  }

  // /// close the file
  template <class charT, class copy>
  inline error_codes::code_type stream<charT, copy>::close() noexcept {
    if(io::open_state::closed == m_openstate) {
      return error_codes::stream::not_open;
    }
    if(EOF == io::file::close(m_file)) {
      return error_codes::stream::go_to_eof;
    }
    m_fname.clear();
    m_file = nullptr;
    m_openmode = io::open_mode::read; // default one ...
    m_openstate = io::open_state::closed;
    // That's all folks!
    return error_codes::stream::success;
  }


  template <class charT, class copy>
  error_codes::code_type stream<charT, copy>::write_record(
    const io::record_header &header,
    const io::record_summary &summary,
    const buffer_type &buffer) {
    // write the record header
    // this header has a fixed size and
    // is by definition 32 bit padded
    if(1 != io::file::write(&header, sizeof(header), 1, m_file)) {
      m_openstate = io::open_state::error;
      return error_codes::stream::bad_write;
    }
    // write the record summary
    // 1) size of the summary
    size_type summary_size = summary.size();
    if(1 != io::file::write(&summary_size, sizeof(size_type), 1, m_file)) {
      m_openstate = io::open_state::error;
      return error_codes::stream::bad_write;
    }
    // 2) the summary
    size_type blk_size = sizeof(io::record_summary::value_type);
    if(summary_size != io::file::write(&summary[0], blk_size, summary_size, m_file)) {
      m_openstate = io::open_state::error;
      return error_codes::stream::bad_write;
    }
    // write the buffer
    size_type buffer_len = buffer.tell();
    if(buffer_len != io::file::write(buffer.begin(), 1, buffer_len, m_file)) {
      m_openstate = io::open_state::error;
      return error_codes::stream::bad_write;
    }
    // Insert any necessary padding to make the next record header start
    // on a four byte boundary in the file (to make it directly accessible
    // for xdr read).
    size_type padding = (4 - (buffer_len & io::marker::align)) & io::marker::align;
    if(padding > 0) {
      if(padding != io::file::write('\0', 1, padding, m_file)) {
        m_openstate = io::open_state::error;
        return error_codes::stream::bad_write;
      }
    }
    // That's all folks!
    return error_codes::stream::success;
  }

}

#endif  //  ACCIO_STREAM_IMPL_H
