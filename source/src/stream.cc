

#include <accio/stream.h>

#include <vector>

namespace accio {
  
  /// open a file
  error_codes::code_type stream::open(const std::string& fn, io::open_mode mode) noexcept {
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
  error_codes::code_type stream::close() noexcept {
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

  error_codes::code_type stream::read_next_record_info(record_info &info) {
    return read_next_record_info(info, true);
  }
  
  
  error_codes::code_type stream::read_next_record_info(record_info &info, bool reset) {
    if((m_openmode != io::open_mode::read) and m_openmode != io::open_mode::read_write) {
      return error_codes::stream::bad_mode;
    }
    if((m_openstate != io::open_state::opened)) {
      return error_codes::stream::not_open;
    }
    // save original pointer location
    auto ori_pos = io::file::tell(m_file);
    std::cout << "ori_pos: " << ori_pos << std::endl;
    // read first 8 bytes
    unsigned char record_header[8] = {0};
    auto readop = io::file::read(record_header, 1, 8, m_file);
    if(readop < 8) {
      return error_codes::stream::eof;
    }
    std::cout << "record_header: " << std::hex << record_header << std::endl;
    // Read record info:
    //  1) The length of the record header.
    //  2) The record marker.
    types::size_type rh_size(0);
    types::marker_type marker(0);
    // TODO debug problem in memcpy !!
    copy_type::memcpy(types::ptr_cast(&rh_size), record_header,   4, 1);
    copy_type::memcpy(types::ptr_cast(&marker),  record_header+4, 4, 1);
    // check record marker
    std::cout << "rh_size: " << rh_size << std::endl;
    std::cout << "marker: " << marker << std::endl;
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
    std::vector<unsigned char> bytes(read_size, 0);
    readop = io::file::read(&bytes[0], 1, read_size, m_file);
    if(readop < read_size) {
      // go back to original position before return
      io::file::seek(m_file, ori_pos, SEEK_SET);
      return error_codes::stream::eof;
    }
    unsigned char* bytes_ptr = &bytes[0];
    types::size_type pos(0), recname_len(0);
    copy_type::memcpy(types::ptr_cast(&info.m_options),  bytes_ptr+pos, sizeof(info.m_options), 1);
    pos += sizeof(info.m_options);
    copy_type::memcpy(types::ptr_cast(&info.m_length),   bytes_ptr+pos, sizeof(info.m_length),  1);
    pos += sizeof(info.m_length);
    copy_type::memcpy(types::ptr_cast(&info.m_clength),  bytes_ptr+pos, sizeof(info.m_clength), 1);
    pos += sizeof(info.m_clength);
    copy_type::memcpy(types::ptr_cast(&recname_len),     bytes_ptr+pos, sizeof(recname_len),    1);
    pos += sizeof(recname_len);
    info.m_name.resize(recname_len);
    char *name_str = &info.m_name[0];
    copy_type::memcpy(types::ptr_cast(name_str),         bytes_ptr+pos, 1,                      1);
    // Restore original position if specified
    if(reset) {
      io::file::seek(m_file, ori_pos, SEEK_SET);
    }
    // that's all folks !
    return error_codes::stream::success;
  }

}
