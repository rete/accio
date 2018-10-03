//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#include <accio/buffer.h>

#define do_test(name, boolean) \
  { \
    if(not (boolean)) { \
      std::cout << "Test: " << name << " failed!" <<  std::endl; \
      return 1; \
    } \
  }

int main() {
  const accio::buffer<unsigned char>::size_type len = 1024*1024;
  accio::buffer<unsigned char> wbuf(len); // 1 Mo buffer, write mode
  do_test("Buffer creation", wbuf.good());

  wbuf.setstate(std::ios_base::eofbit);
  do_test("eofbit", wbuf.eof());
  do_test("not good", not wbuf.good());
  do_test("not badbit", not wbuf.bad());
  do_test("not failbit", not wbuf.fail());
  wbuf.clear_state();

  do_test("buffer size", len == wbuf.size());
  do_test("buffer tell start", 0 == wbuf.tell());
  int wval = 12;
  do_test("buffer write integer", sizeof(int) == wbuf.write_data(wval));

  // read mode, use the previous buffer
  accio::buffer<unsigned char> rbuf(wbuf.begin(), wbuf.tell(), true);
  int rval = 0;
  do_test("buffer read back integer", sizeof(int) == rbuf.read_data(rval));
  do_test("compare write and read", rval == wval);

  std::cout << "TEST_PASSED" << std::endl;
  return 0;
}
