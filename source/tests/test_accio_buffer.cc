//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

// -- accio headers
#include <accio/testing/unit_test.h>
#include <accio/buffer.h>

int main() {

  accio::unit_test test("accio_buffer_test");

  const accio::buffer<unsigned char>::size_type len = 1024*1024;
  accio::buffer<unsigned char> wbuf(len); // 1 Mo buffer, write mode
  test.test("Buffer creation", wbuf.good());

  wbuf.setstate(std::ios_base::eofbit);
  test.test("eofbit", wbuf.eof());
  test.test("not good", not wbuf.good());
  test.test("not badbit", not wbuf.bad());
  test.test("not failbit", not wbuf.fail());
  wbuf.clear_state();

  test.test("buffer size", len == wbuf.size());
  test.test("buffer tell start", 0 == wbuf.tell());
  int wval = 12;
  test.test("buffer write integer", sizeof(int) == wbuf.write_data(wval));

  // read mode, use the previous buffer
  accio::buffer<unsigned char> rbuf(wbuf.begin(), wbuf.tell(), true);
  int rval = 0;
  test.test("buffer read back integer", sizeof(int) == rbuf.read_data(rval));
  test.test("compare write and read", rval == wval);

  std::cout << "TEST_PASSED" << std::endl;
  return 0;
}
