#!/bin/bash

if [ -z ${COVERITY_SCAN_TOKEN} ]
then
  echo "No coverity scan token set !"
  exit 1
fi

export description=`date`
export COVERITY_REPO="rete%2Faccio"

# install coverity scan
wget https://scan.coverity.com/download/Linux --post-data "token=${COVERITY_SCAN_TOKEN}&project=${COVERITY_REPO}" -O coverity_tool.tgz
mkdir cov-analysis-Linux
tar -xf coverity_tool.tgz -C cov-analysis-Linux --strip-components=2 &> /dev/null
export PATH=$PWD/cov-analysis-Linux/bin:$PATH

# run cmake and build with coverity
mkdir -p build
cd build

cmake -DCMAKE_CXX_FLAGS="-Werror -Wextra" -DBUILD_TESTING=ON -DCMAKE_INSTALL_PREFIX=$PWD/.. ..
cov-build --dir cov-int make install
tar czvf myproject.tgz cov-int

# post coverity scan report
curl --form token=${COVERITY_SCAN_TOKEN} --form email=remi.ete@gmail.com --form file=@myproject.tgz --form version="master" --form description="${description}" https://scan.coverity.com/builds?project=${COVERITY_REPO}
