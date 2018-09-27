

#ifndef ACCIO_COPY_IMPL_H
#define ACCIO_COPY_IMPL_H 1

namespace accio {
  
  /// The standard std::memcpy call
  void copy::standard::memcpy(
    buffer_type*       destination,
    const buffer_type* source,
    size_type          size,
    size_type          count) {
    std::memcpy(destination, source, count*size);
  }

  /// Copy data to 'destination' in big endian
  void copy::big_endian::memcpy(
    buffer_type*       destination,
    const buffer_type* source,
    size_type          size,
    size_type          count) {
#ifdef __LITTLE_ENDIAN__
    destination += size;
    for(size_type icnt = 0 ; icnt<count ; icnt++) {
      for(size_type ibyt = 0 ; ibyt<size ; ibyt++) {
        *--destination = *source++;
      }
      destination += (size << 1);
    }
#else
    memcpy(destination, source, size, count);
#endif
  }

  /// Copy data to 'destination' in little endian
  void copy::little_endian::memcpy(
    buffer_type*       destination,
    const buffer_type* source,
    size_type          size,
    size_type          count) {
#ifdef __LITTLE_ENDIAN__
    memcpy(destination, source, size, count);
#else
    destination += size;
    for(size_type icnt = 0 ; icnt<count ; icnt++) {
      for(size_type ibyt = 0 ; ibyt<size ; ibyt++) {
        *--destination = *source++;
      }
      destination += (size << 1);
    }
#endif
  }
}





#endif  //  ACCIO_COPY_IMPL_H
