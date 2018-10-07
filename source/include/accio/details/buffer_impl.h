//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#ifndef ACCIO_BUFFER_IMPL_H
#define ACCIO_BUFFER_IMPL_H 1

#include <accio/definitions.h>

namespace accio {

  /// Allocate a buffer in write mode
  template <class charT, class copy, class alloc>
  template <class>
  inline buffer<charT, copy, alloc>::
  buffer(size_type size) {
    allocator_type allocator;
    m_buffer = allocator.allocate(size);
    m_size = size;
    m_memsize = size;
    std::memset(m_buffer, 0, m_size);
    m_current = m_buffer;
    m_mode = std::ios_base::out | std::ios_base::binary;
  }

  template <class charT, class copy, class alloc>
  template <class>
  inline buffer<charT, copy, alloc>::
  buffer(char_type *bytes, size_type size, bool cpy) {
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

  template <class charT, class copy, class alloc>
  template <class>
  inline buffer<charT, copy, alloc>::
  buffer(FILE *file, size_type size) {
    allocator_type allocator;
    m_buffer = allocator.allocate(size);
    m_current = m_buffer;
    m_size = size;
    m_memsize = size;
    m_mode = std::ios_base::in | std::ios_base::binary;
    // check file ptr first
    if(nullptr == file) {
      setstate(std::ios_base::failbit);
      return;
    }
    // extract n bytes from file
    if(io::file::read(m_buffer, 1, size, file) < size) {
      setstate(std::ios_base::eofbit);
    }
  }

  template <class charT, class copy, class alloc>
  buffer<charT, copy, alloc>::
  buffer(buffer<charT, copy, alloc> &&rhs) {
    // these are not really movable
    m_mode = rhs.m_mode; rhs.m_mode = std::ios_base::out;
    m_iostate = rhs.m_iostate; rhs.m_iostate = std::ios_base::goodbit;
    m_size = rhs.m_size; rhs.m_size = 0;
    m_memsize = rhs.m_memsize; rhs.m_memsize = 0;
    // take the buffer
    m_buffer = rhs.m_buffer; rhs.m_buffer = nullptr;
    m_current = rhs.m_current; rhs.m_current = nullptr;
    // move the maps
    m_pointed_at = std::move(rhs.pointed_at);
    m_pointer_to = std::move(rhs.pointer_to);
  }

  template <class charT, class copy, class alloc>
  inline buffer<charT, copy, alloc>::
  ~buffer() {
    if(nullptr != m_buffer) {
      delete [] m_buffer;
    }
    m_buffer = nullptr;
    m_current = nullptr;
  }

  template <class charT, class copy, class alloc>
  buffer<charT, copy, alloc> &&buffer<charT, copy, alloc>::
  operator=(buffer<charT, copy, alloc> &&rhs) {
    // these are not really movable
    m_mode = rhs.m_mode; rhs.m_mode = std::ios_base::out;
    m_iostate = rhs.m_iostate; rhs.m_iostate = std::ios_base::goodbit;
    m_size = rhs.m_size; rhs.m_size = 0;
    m_memsize = rhs.m_memsize; rhs.m_memsize = 0;
    // take the buffer
    m_buffer = rhs.m_buffer; rhs.m_buffer = nullptr;
    m_current = rhs.m_current; rhs.m_current = nullptr;
    // move the maps
    m_pointed_at = std::move(rhs.pointed_at);
    m_pointer_to = std::move(rhs.pointer_to);
    return *this;
  }

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  bufcpy(char_type *data, size_type size) {
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

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::pos_type buffer<charT, copy, alloc>::
  seekoff(off_type off, seek_dir way) {
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

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  expand(size_type len) {
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

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  read(char_type *data, size_type memlen, size_type count) {
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
    auto total_padded = (total + 3) & 0xfffffffc;
    // reach end of buffer ?
    if(total > rem) {
      total = rem;
      setstate(std::ios_base::eofbit);
    }
    copy_type::memcpy(data, m_current, memlen, count);
    m_current += total_padded;
    return total_padded;
  }

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  write(const char_type *data, size_type memlen, size_type count) {
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
    auto total_padded = (total + 3) & 0xfffffffc;
    if(total_padded > rem) {
      auto expfac = ((total_padded / rem) + 1)*default_expand;
      if(expfac != expand(expfac)) {
        setstate(std::ios_base::failbit);
        return 0;
      }
    }
    copy_type::memcpy(m_current, data, memlen, count);
    m_current += total_padded;
    return total_padded;
  }

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  write_pointer(const address_type *addr) {
    return this->write(addr, 4, 1);
  }

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  read_pointed_at(address_type *addr) {
    address_type *old_address(nullptr);
    auto read_op = this->read(old_address, 4, 1);
    if(4 != read_op) {
      return read_op;
    }
    m_pointed_at.insert(pointed_at::value_type(old_address, addr));
    return read_op;
  }

  template <class charT, class copy, class alloc>
  inline typename buffer<charT, copy, alloc>::size_type buffer<charT, copy, alloc>::
  read_pointer_to(address_type **addr) {
    address_type *old_address(nullptr);
    auto read_op = this->read(old_address, 4, 1);
    if(4 != read_op) {
      return read_op;
    }
    m_pointed_at.insert(pointed_at::value_type(old_address, addr));
    return read_op;
  }

  template <class charT, class copy, class alloc>
  inline bool buffer<charT, copy, alloc>::
  relocate() {
    // check read mode
    if(not (mode() & std::ios_base::in)) {
      setstate(std::ios_base::failbit);
      return false;
    }
    for(auto ptr_ref : m_pointer_to) {
      auto ptr_iter = m_pointed_at.find(ptr_ref.first);
      *(ptr_ref.second) = (ptr_iter != m_pointed_at.end()) ? ptr_iter->second : nullptr;
    }
    m_pointer_to.clear();
    m_pointed_at.clear();
    return true;
  }

}

#endif  //  ACCIO_BUFFER_IMPL_H
