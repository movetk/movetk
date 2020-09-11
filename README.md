# MoveTK: the movement toolkit
![MoveTK-CI-Ubuntu](https://github.com/heremaps/movetk/workflows/MoveTK-CI-Ubuntu/badge.svg?branch=master&event=push) ![MoveTK-CI-macOS](https://github.com/heremaps/movetk/workflows/MoveTK-CI-macOS/badge.svg?branch=master&event=push) ![MoveTK-CI-Windows](https://github.com/heremaps/movetk/workflows/MoveTK-CI-Windows/badge.svg?branch=master&event=push)

MoveTK is a library for computational movement analysis written in C++. The library has been developed as part of a collaboration between [HERE Technologies](https://www.here.com/) , [Eindhoven University of Technology](https://www.tue.nl/en/) and [Utrecht University](https://www.uu.nl/en) under the [Commit2Data](https://commit2data.nl/en/commit2data-program/data-handling/data-science-voor-veranderende-data-2/analysis-and-visualization-of-heterogeneous-spatio-temporal-data) program.

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
| Trajectory Segmentation | Monotone Segmentation [Buchin et al. 2011, Alewijnse  et al. 2014]|
|                         | Model Based Segmentation [Alewijnse et al. 2018] |
| Trajectory Simplification | Douglas-Peucker Simplification [Hershberger et al. 1992,1998] |
|                           | Imai-Iri Simplification [Chan et al. 1996] |
| Trajectory Similarity     | Longest Common Subsequence [Vlachos et al. 2002] |
|                           | Hausdorff & Fréchet  Metric [Eiter et al. 1994, Alt et al. 1995] |
| Trajectory Outlier Detection | Optimal Speed Bounded [Custers et al. 2019] |
|                           | Greedy / Smart Greedy / Local Greedy [Custers et al. 2019] |
| Trajectory Clustering | Sub-trajectory Clustering [Buchin et al. 2008] |
| Trajectory Interpolation | Kinematic Interpolation [Long 2016] |
|                          | Random Trajectory Generator [Technitis et al. 2015] |

## Quick Start
1. Install Docker on your machine see the installation instructions for instance, for [Windows](https://docs.docker.com/docker-for-windows/install/), [Mac](https://docs.docker.com/docker-for-mac/install/) or [Ubuntu Linux](https://docs.docker.com/engine/install/ubuntu/).

2. Clone the repository 

```
git clone https://github.com/heremaps/movetk.git
```

3. Build the docker image

On Mac/Linux

```bash
docker build -t movetk -f  ./docker/vanilla/Dockerfile .
```

On Windows

```bash
docker build -t movetk -f  .\docker\vanilla\Dockerfile .
```

4. Run the container

```bash
docker run -p -it movetk
```

5. Inside the container, navigate to `/usr/src/movetk/Release/examples`
Here you can run some of the examples that are built and shipped with MoveTK.
For example, to convert raw (geo) spatial movement data to trajectories run:

```
./probe2trajectories [path-to-raw-probe.csv[.gz]]
```
    
## Tutorials
1. Install Docker on your machine see the installation instructions for instance, for [Windows](https://docs.docker.com/docker-for-windows/install/), [Mac](https://docs.docker.com/docker-for-mac/install/) or [Ubuntu Linux](https://docs.docker.com/engine/install/ubuntu/).

2. Clone the repository

```bash
git clone https://github.com/heremaps/movetk.git
```

3. Build the Docker image

On Mac/Linux

```bash
docker build -t movetk_with_jupyterlab -f  ./docker/with_jupyterlab/Dockerfile .
```
On Windows

```bash
docker build -t movetk_with_jupyterlab -f  .\docker\with_jupyterlab\Dockerfile .
```

4. Run the container

```bash
docker run -p 6013:8888 -p 80:80 -it movetk_with_jupyterlab
```

5. Follow instructions in the output of step 4 to open Jupyter Notebook in a web browser

```
Copy/paste this URL into your browser when you connect for the first time,
   to login with a token:
       http://(fa5fc67ed6ea or 127.0.0.1):8888/?token=...
```

6. Replace the part in parentheses "()" in step 5 with the IP address and replace port number 8888 with 6013 (or another port depending on your configuration)

Once JupyterLab can be accessed from your browser, in-order to access the notebooks please navigate to

```
/tutorials (using the navigation pane in JupyterLab) 
```

## Third Party Libraries Included In MoveTK

MoveTK utilizes some open source components including:

 1. Modified version of GsTL (see include/third_party/GsTL-1.3)
 2. TNT (Template Numerical Toolkit) (see [include/third_party/GsTL-1.3] folder)
 3. Selection of headers of Boost version 1.70.0 (see [src/include/third_party/boost_future]) 
 4. Miniball (see [src/include/third_party/miniball]) 

## Acknowledgements 
This project includes GsTL which is software developed by Stanford University and
its contributors.

## License
Copyright (C) 2017-2020 HERE Europe B.V.

Unless otherwise noted in `LICENSE` files for specific files or directories, the [LICENSE](LICENSE) in the root applies to all content in this repository.
