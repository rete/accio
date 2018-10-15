[![Build Status](https://travis-ci.org/rete/accio.svg?branch=master)](https://travis-ci.org/rete/accio)
[![Coverity Scan](https://scan.coverity.com/projects/16888/badge.svg)](https://scan.coverity.com/projects/rete-accio)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/2272/badge)](https://bestpractices.coreinfrastructure.org/projects/2272)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/rete/accio.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/rete/accio/context:cpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/37472b4b417d4690a8f2ebef52fe2aa4)](https://www.codacy.com/app/rete/accio?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=rete/accio&amp;utm_campaign=Badge_Grade)

## ACCIO: ACcelerated Compact IO

Provides binary streaming IO using accelerating technology and zlib compression

### Installation

```shell
mkdir build
cd build
cmake [-DOPTIONS=ON/OFF] ../
make install
```

where OPTIONS can be:

  - BUILD_TESTING (default OFF): enable testing (values=ON/OFF)
  - PROFILING (default OFF): enable code profiling (values=ON/OFF)

## Copyright and Licence

Copyright (c) 2018, DESY, Deutsches Elektronen Synchrotron

ACCIO is distributed under the [LGPLv3 License](https://www.gnu.org/licenses/lgpl-3.0.en.html).

## Bug report

Use the [github issues interface](https://github.com/rete/accio/issues)
