//==========================================================================
//  ACCIO: ACelerated and Compact IO library
//--------------------------------------------------------------------------
//
// For the licensing terms see LICENSE file.
// For the list of contributors see AUTHORS file.
//
// Author     : R.Ete
//====================================================================

#ifndef ACCIO_UNITTEST_H
#define ACCIO_UNITTEST_H 1

// -- std headers
#include <string>
#include <iostream>
#include <sstream>

namespace accio {

  class unit_test {
  public:
    unit_test(const std::string &name, bool abort_fail = true) :
    m_name(name),
    m_head("[" + m_name + "] "),
    m_abort_fail(abort_fail) {
      std::cout << m_head << "==================================" << std::endl;
      std::cout << m_head << "== Starting unit test '" << m_name << "'" << std::endl;
      std::cout << m_head << "==================================" << std::endl;
    }

    ~unit_test() {
      print_summary();
    }

    template <typename U, typename V>
    void test(const std::string& test, const U& lhs, const V& rhs) {
      std::stringstream sstr;
      sstr << "[TEST:" << test << "] ";
      if( lhs == rhs ) {
        sstr << "'" << lhs << "' == '" << rhs << "'";
        pass( sstr.str() );
      }
      else {
        sstr << "'" << lhs << "' != '" << rhs << "'";
        error( sstr.str() );
      }
    }

    template <typename C>
    void test(const std::string& test, const C& condition) {
      std::stringstream sstr;
      sstr << "[TEST:" << test << "] ";
      if( (condition) ) {
        sstr << "'" << condition << "'";
        pass( sstr.str() );
      }
      else {
        sstr << "'" << condition << "'";
        error( sstr.str() );
      }
    }

    void error(const std::string& message) {
      m_failed++;
      std::cerr << m_head << message << std::endl;
      if( m_abort_fail ) {
        std::cerr << m_head << "Abort on fail !" << std::endl;
        print_summary();
        exit(1);
      }
    }

    void pass(const std::string& message) {
      m_passed++;
      std::cout << m_head << message << std::endl;
    }

    std::string status() {
      return ( m_failed > 0 ? "TEST_FAILED" : "TEST_PASSED");
    }

    void print_summary() {
      std::cout << m_head << "==================================" << std::endl;
      std::cout << m_head << m_passed << " PASSED, " << m_failed << " FAILED" << std::endl;
      std::cout << m_head << status() << std::endl;
      std::cout << m_head << "==================================" << std::endl;
    }

  private:
    std::string          m_name{};
    std::string          m_head{};
    bool                 m_abort_fail{true};
    unsigned int         m_passed{0};
    unsigned int         m_failed{0};
  };
}

#endif //  ACCIO_UNITTEST_H
