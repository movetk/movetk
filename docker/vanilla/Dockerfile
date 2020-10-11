FROM fedora:30

ARG WITH_CGAL_BACKEND=OFF

# Install dependencies
RUN dnf -y update \
 && if [ "${WITH_CGAL_BACKEND}" = "OFF" ]; then CGAL_PACKAGE=""; else CGAL_PACKAGE="CGAL-devel"; fi \
 && echo WITH_CGAL_BACKEND ${CGAL_PACKAGE} \
 && dnf -y install \
    boost-devel \
    cmake \
    ${CGAL_PACKAGE} \
    doxygen \
    gcc \
    gcc-c++ \
    gsl-devel\
    gdal-devel \
    GeographicLib-devel \
    make \
    mpfr-devel \
    rpm-build \
 && dnf clean all \
 && rm -rf /var/cache/dnf

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/movetk

# Specify the working directory
WORKDIR /usr/src/movetk

RUN mkdir -p Release \
 && cd Release \
 && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local  -DBUILD_DOC=ON  \
 -DWITH_CGAL_BACKEND=${WITH_CGAL_BACKEND} -DBoost_NO_BOOST_CMAKE=ON \
 -DWITH_BOOST=ON -DWITH_GeographicLib=ON .. \
 && cmake --build . -- -j $(nproc) \
 && cmake --build . --target install \
 && make package

LABEL Name=movetk Version=0.0.1