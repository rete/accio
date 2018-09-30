
#include <accio/definitions.h>
#include <type_traits>
#include <iostream>

#define do_test(name, boolean) \
  { \
    if(not (boolean)) { \
      std::cout << "Test: " << name << " failed!" <<  std::endl; \
      return 1; \
    } \
  }


struct valid_pod_1 {
  accio::types::size_type   var1;
  accio::types::size_type   var2;
  accio::types::size_type   len;
  accio::types::string128   str;
};

struct valid_pod_2 {
  valid_pod_1               pod1;
  accio::types::size_type   add;
};

struct invalid_pod_1 {
  accio::types::size_type   var1;
  accio::types::size_type   var2;
  accio::types::size_type   len;
  std::string               str;
};

int main() {
  
  do_test("valid pod 1", std::is_pod<valid_pod_1>::value);
  do_test("valid pod 2", std::is_pod<valid_pod_2>::value);
  do_test("invalid pod 1", not std::is_pod<invalid_pod_1>::value);
  
  do_test("valid pod string 4", std::is_pod<accio::types::string4>::value);
  do_test("valid pod string 8", std::is_pod<accio::types::string8>::value);
  do_test("valid pod string 16", std::is_pod<accio::types::string16>::value);
  do_test("valid pod string 32", std::is_pod<accio::types::string32>::value);
  do_test("valid pod string 64", std::is_pod<accio::types::string64>::value);
  do_test("valid pod string 128", std::is_pod<accio::types::string128>::value);
  do_test("valid pod string 256", std::is_pod<accio::types::string256>::value);
  
  do_test("record header", std::is_pod<accio::io::record_header>::value);
  do_test("block summary", std::is_pod<accio::io::block_summary>::value);

  std::cout << "TEST_PASSED" << std::endl;
  return 0;
}
