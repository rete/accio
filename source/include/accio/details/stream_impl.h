

#ifndef ACCIO_STREAM_IMPL_H
#define ACCIO_STREAM_IMPL_H 1

#include <vector>

namespace accio {

  /// open a file
  inline error_codes::code_type stream::open(const std::string& fn, io::open_mode mode) noexcept {
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

  /// close the file
  inline error_codes::code_type stream::close() noexcept {
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

  inline error_codes::code_type stream::read_next_record_info(record_info &info) {
    return read_next_record_info(info, true);
  }


  inline error_codes::code_type stream::read_next_record_info(record_info &info, bool reset) {
    if((m_openmode != io::open_mode::read) and m_openmode != io::open_mode::read_write) {
      return error_codes::stream::bad_mode;
    }
    if((m_openstate != io::open_state::opened)) {
      return error_codes::stream::not_open;
    }
    // save original pointer location
    auto ori_pos = io::file::tell(m_file);
    // read first 8 bytes
    unsigned char record_header[8] = {0};
    auto readop = io::file::read(record_header, 1, 8, m_file);
    if(readop < 8) {
      return error_codes::stream::eof;
    }
    // Read record info:
    //  1) The length of the record header.
    //  2) The record marker.
    types::size_type rh_size(0);
    types::marker_type marker(0);
    copy_type::memcpy(types::ptr_cast(&rh_size), record_header,   4, 1);
    copy_type::memcpy(types::ptr_cast(&marker),  record_header+4, 4, 1);
    // check record marker
    if(marker != io::marker::record) {
      // go back to original position before return
      io::file::seek(m_file, ori_pos, SEEK_SET);
      return error_codes::stream::no_record_marker;
    }
    // Read record info:
    //  4) The length of the record data (compressed).
    //  5) The length of the record data (uncompressed).
    //  6) The length of the record name.
    //  7) The record name.
    auto read_size = (rh_size-8);
    stream_buffer buf(m_file, read_size);
    if(buf.eof() or buf.fail()) {
      // go back to original position before return
      io::file::seek(m_file, ori_pos, SEEK_SET);
      return error_codes::stream::eof;
    }
    types::size_type recname_len(0);
    buf.read_data(info.m_options, 1);
    buf.read_data(info.m_clength, 1);
    buf.read_data(info.m_length, 1);
    buf.read_data(recname_len, 1);
    info.m_name.resize(recname_len);
    buf.read_data(info.m_name[0], recname_len);
    // Restore original position if specified
    if(reset) {
      io::file::seek(m_file, ori_pos, SEEK_SET);
    }
    // that's all folks !
    return error_codes::stream::success;
  }
  
  error_codes::code_type stream::skip_next_record(record_info &info) {
    auto status = read_next_record_info(info, false);
    auto skip_len = info.m_clength + ((4 - (info.m_clength & io::marker::align)) & io::marker::align);
    if(error_codes::stream::success != status) {
      return status;
    }
    if(io::file::seek(m_file, skip_len, SEEK_CUR)) {
      return error_codes::stream::off_end;
    }
    return error_codes::stream::success;
  }

}

#endif  //  ACCIO_STREAM_IMPL_H