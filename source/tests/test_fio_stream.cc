
#include <fio/stream.h>

#define do_test(name, boolean) \
  { \
    if(not (boolean)) { \
      std::cout << "Test: " << name << " failed!" <<  std::endl; \
      return 1; \
    } \
  }

int main(int argc, char **argv) {
  fio::stream fstream("TestStream");
  
  if(argc != 2) {
    return 1;
  }
  const std::string fname = argv[1];
  std::cout << "opening input file " << fname << std::endl;
  do_test("opening input file", fio::error_codes::stream::success == fstream.open(fname, fio::io::open_mode::read));
  fio::record_info rinfo;
  std::cout << "read 1 rst record" << std::endl;
  do_test("read 1 rst record", fio::error_codes::stream::success == fstream.read_next_record_info(rinfo));
  std::cout << "== 1rst record in file ==" << std::endl;
  std::cout << "   - option word: " << rinfo.m_options << std::endl;
  std::cout << "   - len:         " << rinfo.m_length << std::endl;
  std::cout << "   - clen:        " << rinfo.m_clength << std::endl;
  std::cout << "   - name:        " << rinfo.m_name << std::endl;
  

  // do_test("compare write and read", rval == wval);

  std::cout << "TEST_PASSED" << std::endl;
  return 0;
}
