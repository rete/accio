//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#ifndef ACCIO_WRITER_IMPL_H
#define ACCIO_WRITER_IMPL_H 1

namespace accio {

  /// Open a file in write mode
  template <typename config>
  error_codes::code_type file_writer<config>::open(const std::string &fname) {
    return m_stream.open(fname, io::open_mode::write_new);
  }

  // write a record
  template <typename config>
  error_codes::code_type file_writer<config>::write_record(
    const string32 &name,
    const record_io &io_config,
    const record_type &rec) {
      // check stream state
    if(m_stream.open_state() != io::open_state::opened) {
      return error_codes::stream::not_open;
    }
    // create block writers from user record config
    block_writers writers;
    auto status = io_config.create_writers(rec, writers);
    // tools for writting
    io::record_summary rec_summary;
    io::record_header rec_header;
    buffer<char_type, copy_type> outbuf;
    // fill the record header
    rec_header.m_marker = io::marker::record;
    rec_header.m_options = 0; // TODO fill this !
    rec_header.m_compsize = 0;
    rec_header.m_uncompsize = 0; // TODO deal with compression !
    rec_header.m_name = name;
    // write blocks in the buffer
    for(auto writer : writers) {
      auto begin_pos = outbuf.tell();
      auto status = writer->write(outbuf);
      if(error_codes::block::success != status) {
        std::cout << "ERROR - Couldn't write block:" << std::endl;
        std::cout << "  => type: " << writer->type() << std::endl;
        std::cout << "  => name: " << writer->name() << std::endl;
        std::cout << "  => version: " << writer->version() << std::endl;
        std::cout << "Skipping ..." << std::endl;
        outbuf.seekpos(begin_pos);
        continue;
      }
      auto new_pos = outbuf.tell();
      if(new_pos <= begin_pos) {
        std::cout << "ERROR - Invalid buffer pointer after block writing:" << std::endl;
        std::cout << "  => type: " << writer->type() << std::endl;
        std::cout << "  => name: " << writer->name() << std::endl;
        std::cout << "  => version: " << writer->version() << std::endl;
        std::cout << "Skipping ..." << std::endl;
        outbuf.seekpos(begin_pos);
        continue;
      }
      // fill the block summary and add it to record summary
      io::block_summary blk_summary;
      blk_summary.m_version = writer->version();
      blk_summary.m_type = writer->type();
      blk_summary.m_name = writer->name();
      blk_summary.m_size = (new_pos - begin_pos);
      rec_summary.push_back(blk_summary);
      // fill un-compressed total size
      rec_header.m_uncompsize = new_pos;
    }
    // TODO deal with compression
    rec_header.m_compsize = rec_header.m_uncompsize;
    m_stream.write_record(rec_header, rec_summary, outbuf);
  }

}



#endif  //  ACCIO_WRITER_IMPL_H
