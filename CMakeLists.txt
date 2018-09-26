#################################
# cmake file for building accio
# @author Remi Ete, DESY
#################################
cmake_minimum_required(VERSION 3.4)

# project name
project( accio )

# project version
set( accio_VERSION_MAJOR 0 )
set( accio_VERSION_MINOR 1 )
set( accio_VERSION_PATCH 0 )

# library *nix style versioning
set( accio_SOVERSION "${accio_VERSION_MAJOR}.${accio_VERSION_MINOR}" )
set( accio_VERSION "${accio_SOVERSION}.${accio_VERSION_PATCH}" )

# set default install prefix to project root directory
# instead of the cmake default
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  set( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
endif()

message( STATUS "CMAKE_INSTALL_PREFIX set to ${CMAKE_INSTALL_PREFIX}" )

# check c++ standard requirements
include( CheckCXXCompilerFlag )

check_cxx_compiler_flag( "-std=c++14" COMPILER_SUPPORTS_CXX14 )

if( COMPILER_SUPPORTS_CXX14 )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
else()
  message( SEND_ERROR "${PROJECT_NAME} requires C++14 support. Please upgrade your compiler !" )
endif()

find_package( ZLIB REQUIRED )
#ZLIB_INCLUDE_DIR - where to find zlib.h, etc.
#ZLIB_LIBRARIES   - List of libraries when using zlib.
#ZLIB_FOUND       - True if zlib found.

include( TestBigEndian )
test_big_endian( BIG_ENDIAN )

if( NOT BIG_ENDIAN )
  add_definitions( "-D__LITTLE_ENDIAN__" )
  message( STATUS "Plateform is little endian" )  
else()
  message( STATUS "Plateform is big endian" )
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/source/include
  ${ZLIB_INCLUDE_DIR}
)

aux_source_directory( ${PROJECT_SOURCE_DIR}/source/src accio_SRCS )

add_library( accio SHARED ${accio_SRCS} )
target_link_libraries( accio ${ZLIB_LIBRARIES} )
install( TARGETS accio LIBRARY DESTINATION lib )

install( DIRECTORY ${PROJECT_SOURCE_DIR}/source/include DESTINATION . )

# enable ctest
ENABLE_TESTING()
INCLUDE(CTest)
MARK_AS_ADVANCED( DART_TESTING_TIMEOUT )

macro( add_accio_test file )
  if( BUILD_TESTING )
    add_executable( ${file} ${PROJECT_SOURCE_DIR}/source/tests/${file}.cc )
  else()
    add_executable( ${file} EXCLUDE_FROM_ALL ${PROJECT_SOURCE_DIR}/source/tests/${file}.cc )
  endif()
#  ADD_DEPENDENCIES( tests ${file} )
  target_link_libraries( ${file} accio )
  add_test( t_${file} "${EXECUTABLE_OUTPUT_PATH}/${file}" )
  set_tests_properties( t_${file} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
endmacro()

add_accio_test( test_fio_buffer )

add_executable( test_fio_stream ${PROJECT_SOURCE_DIR}/source/tests/test_fio_stream.cc )
target_link_libraries( test_fio_stream accio )
