//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#include <accio/testing/unit_test.h>
#include <accio/string.h>
#include <type_traits>

int main() {

  accio::unit_test test("accio_string_test");

  test.test("sizeof string8 is 8", 8 == sizeof(accio::string8));
  test.test("sizeof string32 is 32", 32 == sizeof(accio::string32));
  test.test("sizeof string64 is 64", 64 == sizeof(accio::string64));
  test.test("sizeof string128 is 128", 128 == sizeof(accio::string128));

  accio::string8 str8;
  test.test("size is zero", 0 == str8.size());
  test.test("capacity is 8", 8 == str8.capacity());
  test.test("max size is 7", 7 == str8.max_size());

  str8 = "toto"; // 4 characters
  test.test("size is 4", 4 == str8.size());

  str8 = "tototatat"; // 9 characters
  test.test("size is not 9", 9 != str8.size());
  test.test("size is 7", 7 == str8.size());
  test.test("equality operator", str8 == "tototat");
  test.test("inequality operator", str8 != "tototet");
  test.test("char at 5 is t", str8.at(5) == 'a');
  test.test("char [5] is t", str8[5] == 'a');

  try {
    auto c = str8.at(8);
    test.test("out of range", false);
  }
  catch(std::out_of_range &except) {
    // we must end up here
    test.test("out of range", true);
  }

  return 0;
}
