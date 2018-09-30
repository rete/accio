
#include <accio/stream.h>
#include <accio/writer.h>

int main(int argc, char **argv) {
  // accio::stream fstream;
  // 
  // if(argc != 2) {
  //   return 1;
  // }
  // const std::string fname = argv[1];
  // std::cout << "Read records from file " << fname << std::endl;
  // if(accio::error_codes::stream::success != fstream.open(fname, accio::io::open_mode::read)) {
  //   std::cout << "Couldn't open file '" << fname << "'" << std::endl;
  //   return 1;
  // }
  // 
  // unsigned int rec_id = 0;
  // 
  // while(1) {
  //   accio::record_info rinfo;
  //   auto status = fstream.skip_next_record(rinfo);
  // 
  //   if(accio::error_codes::stream::success != status) {
  //     break;
  //   }
  // 
  //   std::cout << "== record no " << rec_id << " ==" << std::endl;
  //   std::cout << "   - option word: " << rinfo.m_options << std::endl;
  //   std::cout << "   - len:         " << rinfo.m_length << std::endl;
  //   std::cout << "   - clen:        " << rinfo.m_clength << std::endl;
  //   std::cout << "   - name:        " << rinfo.m_name << std::endl;
  // 
  //   rec_id++;
  // }

  return 0;
}
