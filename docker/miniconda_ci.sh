#!/bin/bash

###################
# find scripts dir
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
###################

cd ${DIR}
if [ -d Release ]; then
    echo 'Removing existing Release directory and re-creating it...';
    rm -rf Release
fi
mkdir Release
cd Release/
/miniconda/bin/cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/local -DBUILD_DOC=OFF -DWITH_CGAL_BACKEND=OFF -DBoost_NO_BOOST_CMAKE=ON \
  -DCMAKE_CXX_COMPILER=/miniconda/bin/x86_64-conda_cos6-linux-gnu-g++ -DCMAKE_C_COMPILER=/miniconda/bin/x86_64-conda_cos6-linux-gnu-gcc \
   -DCMAKE_CXX_STANDARD=17 ..
/miniconda/bin/cmake --build . -- -j 2
ctest -VV
/miniconda/bin/cmake --build . --target install
#cd .. && rm -rf Release

#make package