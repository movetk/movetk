FROM fedora:latest

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
    python3 \
    python3-pip \
    bzip2 \
    wget \
    nodejs \
    net-tools \
    unzip \
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

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/movetk
#COPY ./pytulib /usr/src/pytulib

# Specify the working directory
WORKDIR /usr/src/movetk

RUN mkdir -p Release \
 && cd Release \
 && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_DOC=ON  \
 -DWITH_CGAL_BACKEND=${WITH_CGAL_BACKEND} -DBoost_NO_BOOST_CMAKE=ON \
 -DWITH_BOOST=ON -DWITH_GeographicLib=ON .. \
 && cmake --build . -- -j $(nproc) \
 && cmake --build . --target install

# Install miniconda to /miniconda
RUN curl -LO http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
RUN bash Miniconda3-latest-Linux-x86_64.sh -p /miniconda -b
RUN rm Miniconda3-latest-Linux-x86_64.sh
ENV PATH=/miniconda/bin:${PATH}
RUN conda update -y conda

# Python packages from conda
RUN conda install -c conda-forge -y \
    xeus-cling=0.9.0=he513fc3_0 \
	libboost \
	mpfr \
	libgdal \
	gsl
	
RUN pip3 --no-cache-dir install jupyterlab
# RUN jupyter kernelspec install /miniconda/share/jupyter/kernels/xcpp11
# RUN jupyter kernelspec install /miniconda/share/jupyter/kernels/xcpp14
RUN jupyter kernelspec install /miniconda/share/jupyter/kernels/xcpp17
RUN cp /usr/src/movetk/docker/with_jupyterlab/kernel.json /usr/local/share/jupyter/kernels/xcpp17/
RUN cp /usr/src/movetk/docker/with_jupyterlab/movetk.json /miniconda/etc/xeus-cling/tags.d/
RUN cp /usr/src/movetk/docs/movetk.tag /miniconda/share/xeus-cling/tagfiles/
RUN mkdir /usr/local/include/GeographicLib
RUN cp /usr/include/GeographicLib/* /usr/local/include/GeographicLib/ 

# Download GeoLife data
WORKDIR /usr/src/movetk/tutorials
RUN wget https://download.microsoft.com/download/F/4/8/F4894AA5-FDBC-481E-9285-D5F8C4C4F039/Geolife%20Trajectories%201.3.zip
RUN unzip Geolife\ Trajectories\ 1.3.zip
WORKDIR /usr/src
RUN mkdir /var/www/html/movetk.reference
RUN cp -R movetk/docs/html/* /var/www/html/movetk.reference/

ENTRYPOINT httpd && jupyter lab --ip 0.0.0.0 --allow-root && /bin/bash

LABEL Name=movetk Version=0.6.0
