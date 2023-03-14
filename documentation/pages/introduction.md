# The Movement Toolkit {#mainpage}

# MoveTK: the movement toolkit

MoveTK is a library for computational movement analysis written in C++. The library has been developed as part of a collaboration between [HERE Technologies](https://www.here.com/) , [Eindhoven University of Technology](https://alga.win.tue.nl/) and [Utrecht University](https://www.uu.nl/en/research/algorithms/geometric-computing) under the [Commit2Data](https://commit2data.nl/en/commit2data-program/data-handling/data-science-voor-veranderende-data-2/analysis-and-visualization-of-heterogeneous-spatio-temporal-data) program.

## Overview
MoveTK is a reusable, well tested, high performant, feature-rich library that incorporates state of the art algorithms for understanding movement.
The library is designed to be extensible such that it can be adapted to various movement analysis tasks.
It's design is inspired by the designs of [CGAL](https://github.com/CGAL/cgal) and [GUDHI](https://github.com/GUDHI/gudhi-devel).

You can use this library to:

- Develop applications for processing (geo) spatial movement data at scale
- Create high performance applications for analysis of movement data, using the algorithms available in the library

Some examples are shown below:

![movetk_algorithm_visualization](docs/images/algorithm_visualization.png)

The following table lists some of the algorithms available in MoveTK

| Feature | Algorithm |
| ------- | --------- |
| **Trajectory Segmentation** | Monotone Segmentation [[Buchin et al. 2011](http://josis.org/index.php/josis/article/view/66), [Alewijnse  et al. 2014](https://dl.acm.org/doi/10.1145/2666310.2666415)]|
|                         | Model Based Segmentation [[Alewijnse et al. 2018](https://link.springer.com/article/10.1007%2Fs00453-017-0329-x)] |
| **Trajectory Simplification** | Douglas-Peucker Simplification [[Hershberger et al. 1992,1998](https://dl.acm.org/doi/book/10.5555/902273)] |
|                           | Imai-Iri Simplification [[Chan et al. 1996](https://www.worldscientific.com/doi/abs/10.1142/S0218195996000058)] |
|                           | Agarwal Simplification [[Agarwal et al. 2005](https://doi.org/10.1007/s00453-005-1165-y)] |
| **Trajectory Similarity**     | Longest Common Subsequence [[Vlachos et al. 2002](https://ieeexplore.ieee.org/document/994784)] |
|                           | Hausdorff & Fréchet  Metric [[Eiter et al. 1994, Alt et al. 1995](https://www.worldscientific.com/doi/abs/10.1142/S0218195995000064)] |
| **Trajectory Outlier Detection** | Optimal Speed Bounded [[Custers et al. 2019](https://dl.acm.org/doi/10.1145/3347146.3359363)] |
|                           | Greedy / Smart Greedy / Local Greedy [[Custers et al. 2019](https://dl.acm.org/doi/10.1145/3347146.3359363)] |
| **Trajectory Clustering** | Sub-trajectory Clustering [[Buchin et al. 2008](https://link.springer.com/chapter/10.1007%2F978-3-540-92182-0_57)] |
| **Trajectory Interpolation** | Kinematic Interpolation [[Long 2016](https://www.tandfonline.com/doi/abs/10.1080/13658816.2015.1081909?journalCode=tgis20)] |
|                          | Random Trajectory Generator [[Technitis et al. 2015](https://www.tandfonline.com/doi/abs/10.1080/13658816.2014.999682?journalCode=tgis20)] |

## Supported compilers
MoveTK has been tested with
* MSVC 14.29.30133 (VS2019)
* GCC 11.3.0


## Building MoveTK 
MoveTK uses the well-established build tool CMake for generating its build files for different generators and OSes. To download and build MoveTK, follow the below steps:
1. Clone the repository in your desired directory, let this directory be ``<movetk_root>``
```bash
git clone --recursive https://github.com/heremaps/movetk.git
```
2. Install the dependencies of MoveTK. The following strategies are supported:
<details>
  <summary><b>Installing dependencies via apt (Ubuntu 22.04)</b></summary>
For Ubuntu, install the following packages using apt 

```sh
sudo apt install libboost-all-dev libgsl-dev libgslcblas0 gsl-bin libgsl27 
```
For building the documentation, you also need [Doxygen](https://www.doxygen.nl/)
```sh
sudo apt install doxygen
```
For the CGAL backend, you also need the following dependencies
```sh
sudo apt install libmpfr-dev libcgal-dev
```
</details>
<details>
  <summary><b>Installing dependencies via vcpkg (Windows/Linux)</b></summary>

MoveTK provides a vcpkg.json manifest file for use with [vcpkg](https://vcpkg.io/). To install the dependencies, you can either let vcpkg handle it during the CMake generation step in Step 3., or directly call the following inside ``<movetk_root>``:
```sh
vcpkg.exe install 
```
During the CMake generation step in Step 3, you need to add ``-DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake`` to the CMake command line arguments, where ``<vcpkg_root>`` is the root of your vcpkg installation.


</details>
<details>
  <summary><b>Installing dependencies via conan (Windows/Linux)</b></summary>

MoveTK also provides a conanfile.txt to install its dependencies using [Conan](https://conan.io/). For this, install a conan version less than 2.0 (2.0 is at the time of writing the default version when installed via ``pip``, but the dependencies are not up to speed yet).
You need a profile for conan to install packages. This profile can be created via the ``conan profile`` subcommands, see [https://docs.conan.io/1/reference/commands/misc/profile.html](https://docs.conan.io/1/reference/commands/misc/profile.html). 
To install the dependencies, run
```sh
conan install <movetk_root>/conanfile.txt -if <installation_folder_for_conan> --build=missing
```
where ``<installation_folder_for_conan>`` is some folder you specify where files for finding the dependencies are written.

Finally, add ``-DCMAKE_PREFIX_PATH=<installation_folder_for_conan>`` to the end of the CMake invocation at Step 3. This way, CMake can find the dependencies and link them with MoveTK.

> Note: if you use a single configuration generator for CMake, such as Ninja or Make, also add the explicit build type to the invocation at Step 3 using ``-DCMAKE_BUILD_TYPE=<build_type>``, with ``<build_type`` a CMake build type such as ``Release``. Make sure to match this build type with a build type that you installed the dependencies for (which is determined by the profile).
</details>
> Note: the vcpkg and conan installation files contain the dependencies for both the Boost and CGAL backend. If you do not want to use the CGAL backend, you need to manually remove the dependency from the respective dependency file.

3. Generate the MoveTK build files
```sh
cmake -S <movetk_root> -B <build_folder> <dependency_options>
```
with ``<build_folder`` your favourite buildfolder for MoveTK and ``<dependency_options>`` any options required by the installation of the dependencies, e.g. ``-DCMAKE_PREFIX_PATH=<conan_install_folder>`` for the conan approach.

4. Build MoveTK. Run the following from the build folder
```sh
cmake --build .
```

### Building tests and examples
To build the tests and/or examples, add ``-DMOVETK_BUILD_TESTS=ON`` resp. ``-DMOVETK_MOVETK_BUILD_EXAMPLES`` to the CMake generation step (Step 3).

### Selecting backends
MoveTK currently supplies two geometry backends that you can choose from: one based on Boost geometry and one on CGAL. By default, MoveTK only builds with the Boost backend enable. The CGAL backend can be separately enabled by adding ``-DMOVETK_WITH_CGAL_BACKEND=ON`` to the CMake generation step. To disable the Boost backend, you can add ``-DMOVETK_WITH_BOOST_BACKEND=OFF`` to the generation step.

## Using MoveTK in your App

Please refer to this [example](https://github.com/aniketmitra001/movetk-app-template) for a template on how to use MoveTK in your app 

In addition, if you add the MoveTK CMake to your own project via ``add_subdirectory``, all you have to do is link with the ``movetk`` target.

## Third Party Libraries Included In MoveTK

MoveTK utilizes some open source components including:

 1. Modified version of GsTL (see include/third_party/GsTL-1.3)
 2. TNT (Template Numerical Toolkit) (see [include/third_party/GsTL-1.3] folder)
 3. Selection of headers of Boost version 1.70.0 (see [src/include/third_party/boost_future]) 
 4. Miniball (see [src/include/third_party/miniball])
 5. rapidjson (see include/third_party/rapidjson)
MoveTK 

## Acknowledgements 
This project includes GsTL which is software developed by Stanford University and
its contributors.

## License
Copyright (C) 2017-2020 HERE Europe B.V.

Unless otherwise noted in `LICENSE` files for specific files or directories, the [LICENSE](LICENSE) in the root applies to all content in this repository. Note that CGAL is largely GPL, so choosing the CGAL backend may affect the license under which MoveTK is available.

