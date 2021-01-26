FROM continuumio/miniconda3

ARG WITH_CGAL_BACKEND=OFF

# Configure as a web server 
RUN apt-get install -q -y  \
    apache2 \
    unzip \
    && apt-get clean

RUN  /etc/init.d/apache2 start
EXPOSE 80

CMD ["/usr/sbin/httpd", "-D", "FOREGROUND"]

# Install dependencies
RUN /opt/conda/bin/conda install  gcc_linux-64\
    gxx_impl_linux-64 \
    boost \
    cmake \
    gsl \
    make \
    mpfr \
    jupyterlab

RUN /opt/conda/bin/conda install -c conda-forge -y xeus-cling=0.9.0=he513fc3_0 \
    gdal \
    geographiclib \
    awscli \
    doxygen

# Build GeographicLib
WORKDIR /usr/src/

RUN wget https://downloads.sourceforge.net/project/geographiclib/distrib/GeographicLib-1.50.tar.gz
RUN tar xfpz GeographicLib-1.50.tar.gz
RUN cd /usr/src/GeographicLib-1.50
RUN mkdir /usr/src/GeographicLib-1.50/BUILD
WORKDIR /usr/src/GeographicLib-1.50/BUILD/

RUN /opt/conda/bin/cmake  -DCMAKE_C_COMPILER=/opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc \
     -DCMAKE_CXX_COMPILER=/opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++ \
     -DCMAKE_CXX_STANDARD=17 /usr/src/GeographicLib-1.50/

RUN /opt/conda/bin/make && /opt/conda/bin/make test \
    && /opt/conda/bin/make install

RUN cp /usr/local/lib/libGeographic.so*  /opt/conda/lib/

# Build MoveTK
COPY . /usr/src/movetk

RUN mkdir /usr/src/movetk/Release/
WORKDIR /usr/src/movetk/Release/

RUN /opt/conda/bin/cmake  -DCMAKE_BUILD_TYPE=Release  \
    -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_DOC=ON  -DWITH_CGAL_BACKEND=OFF \
    -DBoost_NO_BOOST_CMAKE=ON  \
    -DCMAKE_CXX_COMPILER=/opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++ \
    -DCMAKE_C_COMPILER=/opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc \
    -DCMAKE_CXX_STANDARD=17 ..

RUN /opt/conda/bin/cmake --build . -- -j 2 \
    && /opt/conda/bin/cmake --build . --target install

# Configure Jupyter Lab
RUN jupyter kernelspec install /opt/conda/share/jupyter/kernels/xcpp17
RUN cp /usr/src/movetk/docker/with_jupyterlab/xcpp17/kernel.json /opt/conda/share/jupyter/kernels/xcpp17/

RUN cp /usr/src/movetk/docker/with_jupyterlab/movetk.json /opt/conda/etc/xeus-cling/tags.d/

RUN cp /usr/src/movetk/docs/movetk.tag /opt/conda/share/xeus-cling/tagfiles/

# Download GeoLife data
WORKDIR /usr/src/movetk/tutorials
RUN wget https://download.microsoft.com/download/F/4/8/F4894AA5-FDBC-481E-9285-D5F8C4C4F039/Geolife%20Trajectories%201.3.zip
RUN unzip Geolife\ Trajectories\ 1.3.zip
WORKDIR /usr/src
RUN mkdir /var/www/html/movetk.reference
RUN cp -R movetk/docs/html/* /var/www/html/movetk.reference/

ENTRYPOINT jupyter lab --ip 0.0.0.0 --allow-root && /bin/bash

LABEL Name=movetk Version=0.6.0
