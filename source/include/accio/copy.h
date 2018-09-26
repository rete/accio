
#ifndef FIO_COPY_H
#define FIO_COPY_H 1

// -- std headers
#include <cstring>

namespace fio {

  struct copy {
    struct standard {
    public:
      typedef unsigned char buffer_type;
      typedef std::size_t   size_type;

      /// The standard std::memcpy call
      /// plateform endianess -> plateform endianess
      static void memcpy(
        buffer_type*       destination,
        const buffer_type* source,
        size_type          size,
        size_type          count);
    };

    struct big_endian {
    public:
      typedef unsigned char buffer_type;
      typedef std::size_t   size_type;

      /// Copy data to 'destination' in big endian
      /// plateform endianess -> big endian
      static void memcpy(
        buffer_type*       destination,
        const buffer_type* source,
        size_type          size,
        size_type          count);
    };

    struct little_endian {
    public:
      typedef unsigned char buffer_type;
      typedef std::size_t   size_type;

      /// Copy data to 'destination' in little endian
      /// plateform endianess -> little endian
      static void memcpy(
        buffer_type*       destination,
        const buffer_type* source,
        size_type          size,
        size_type          count);
    };
  };
}

#endif
