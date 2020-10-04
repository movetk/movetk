FROM fedora:latest

ARG WITH_CGAL_BACKEND=OFF

# Install dependencies
RUN dnf -y update \
&& if [ "${WITH_CGAL_BACKEND}" = "OFF" ]; then CGAL_PACKAGE=""; else CGAL_PACKAGE="CGAL-devel"; fi \
 && echo WITH_CGAL_BACKEND ${CGAL_PACKAGE} \
 && dnf -y install \
    bzip2 \
    cmake \ 
    make \ 
    wget \
    nodejs \
    gcc \
    gcc-c++ \
    net-tools \
    unzip \
    ${CGAL_PACKAGE} \
 && dnf clean all \
 && rm -rf /var/cache/dnf

# Install web server
RUN dnf -y update && dnf -y install httpd && dnf -y clean all
RUN [ -d /var/log/httpd ] || mkdir /var/log/httpd
RUN [ -d /var/run/httpd ] || mkdir /var/run/httpd
RUN [ -d /var/lock/httpd ] || mkdir /var/lock/httpd
RUN sed -i.orig 's/#ServerName/ServerName/' /etc/httpd/conf/httpd.conf

ENV APACHE_RUN_USER apache
ENV APACHE_RUN_GROUP apache
ENV APACHE_LOG_DIR /var/log/httpd
ENV APACHE_LOCK_DIR /var/lock/httpd
ENV APACHE_RUN_DIR /var/run/httpd
ENV APACHE_PID_FILE /var/run/httpd/httpd.pid

EXPOSE 80

CMD ["/usr/sbin/httpd", "-D", "FOREGROUND"]

# Install miniconda to /miniconda
RUN curl -LO http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
RUN bash Miniconda3-latest-Linux-x86_64.sh -p /miniconda -b
RUN rm Miniconda3-latest-Linux-x86_64.sh
ENV PATH=/miniconda/bin:${PATH}
RUN conda update -y conda

RUN conda search python 

# Python packages from conda
RUN conda install -c conda-forge -y \
    python=3.6.12=hcff3b4d_2 \
    xeus-cling=0.9.0=he513fc3_0 \
	libboost \
	mpfr \
	libgdal \
	gsl \
    cmake \
    make \
    gcc_linux-64 \
    pybind11 \
    geographiclib \
    doxygen



WORKDIR /usr/src/

# Install GeographicLib
RUN wget https://downloads.sourceforge.net/project/geographiclib/distrib/GeographicLib-1.50.tar.gz
RUN tar xfpz GeographicLib-1.50.tar.gz
RUN cd /usr/src/GeographicLib-1.50
RUN mkdir /usr/src/GeographicLib-1.50/BUILD
WORKDIR /usr/src/GeographicLib-1.50/BUILD/
RUN /miniconda/bin/cmake -DCMAKE_INSTALL_PREFIX=/miniconda -DCMAKE_C_COMPILER=/miniconda/bin/x86_64-conda_cos6-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=/miniconda/bin/x86_64-conda_cos6-linux-gnu-g++ \
    -DCMAKE_CXX_STANDARD=17 /usr/src/GeographicLib-1.50/
RUN /miniconda/bin/make
RUN /miniconda/bin/make test
RUN /miniconda/bin/make install
#RUN mv /usr/local/include/GeographicLib /miniconda/include/

# Set the library path
RUN cp -R /miniconda/lib/* /miniconda/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib/
#RUN cp /usr/local/lib/libGeographic.so* /miniconda/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib/
RUN export LD_LIBRARY_PATH=/miniconda/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib/
RUN ldconfig
RUN export LIBRARY_PATH=/miniconda/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib/

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/movetk

# Specify the working directory
WORKDIR /usr/src/movetk


# RUN mkdir -p Release \
#  && cd Release \
#  && /miniconda/bin/cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_DOC=ON  -DWITH_CGAL_BACKEND=${WITH_CGAL_BACKEND} -DBoost_NO_BOOST_CMAKE=ON  \
#  -DCMAKE_CXX_COMPILER=/miniconda/bin/x86_64-conda_cos6-linux-gnu-g++ -DCMAKE_C_COMPILER=/miniconda/bin/x86_64-conda_cos6-linux-gnu-gcc \
#  -DCMAKE_CXX_STANDARD=17 -DCMAKE_PREFIX_PATH=/miniconda -DCMAKE_PREFIX_PATH=/miniconda -DPython_EXECUTABLE=/miniconda/bin/python3.6m .. \
# && /miniconda/bin/cmake --build . -- -j $(nproc) \
# && /miniconda/bin/cmake --build . --target install

	
# RUN pip3 --no-cache-dir install jupyterlab
# # RUN jupyter kernelspec install /miniconda/share/jupyter/kernels/xcpp11
# # RUN jupyter kernelspec install /miniconda/share/jupyter/kernels/xcpp14
# RUN jupyter kernelspec install /miniconda/share/jupyter/kernels/xcpp17
# RUN cp /usr/src/movetk/docker/with_jupyterlab/kernel.json /usr/local/share/jupyter/kernels/xcpp17/
# RUN cp /usr/src/movetk/docker/with_jupyterlab/movetk.json /miniconda/etc/xeus-cling/tags.d/
# RUN cp /usr/src/movetk/docs/movetk.tag /miniconda/share/xeus-cling/tagfiles/
# RUN mkdir /usr/local/include/GeographicLib
# RUN cp /usr/include/GeographicLib/* /usr/local/include/GeographicLib/ 

# # Download GeoLife data
# WORKDIR /usr/src/movetk/tutorials
# RUN wget https://download.microsoft.com/download/F/4/8/F4894AA5-FDBC-481E-9285-D5F8C4C4F039/Geolife%20Trajectories%201.3.zip
# RUN unzip Geolife\ Trajectories\ 1.3.zip
# WORKDIR /usr/src
# RUN mkdir /var/www/html/movetk.reference
# RUN cp -R movetk/docs/html/* /var/www/html/movetk.reference/

# ENTRYPOINT httpd && jupyter lab --ip 0.0.0.0 --allow-root && /bin/bash

LABEL Name=movetk Version=0.6.0
