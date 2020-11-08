# MoveTK: the movement toolkit
![MoveTK-CI-Ubuntu](https://github.com/heremaps/movetk/workflows/MoveTK-CI-Ubuntu/badge.svg?branch=master&event=push) ![MoveTK-CI-macOS](https://github.com/heremaps/movetk/workflows/MoveTK-CI-macOS/badge.svg?branch=master&event=push) ![MoveTK-create-docker-image](https://github.com/heremaps/movetk/workflows/MoveTK-create-docker-image/badge.svg?branch=master)

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
| **Trajectory Similarity**     | Longest Common Subsequence [[Vlachos et al. 2002](https://ieeexplore.ieee.org/document/994784)] |
|                           | Hausdorff & Fréchet  Metric [[Eiter et al. 1994, Alt et al. 1995](https://www.worldscientific.com/doi/abs/10.1142/S0218195995000064)] |
| **Trajectory Outlier Detection** | Optimal Speed Bounded [[Custers et al. 2019](https://dl.acm.org/doi/10.1145/3347146.3359363)] |
|                           | Greedy / Smart Greedy / Local Greedy [[Custers et al. 2019](https://dl.acm.org/doi/10.1145/3347146.3359363)] |
| **Trajectory Clustering** | Sub-trajectory Clustering [[Buchin et al. 2008](https://link.springer.com/chapter/10.1007%2F978-3-540-92182-0_57)] |
| **Trajectory Interpolation** | Kinematic Interpolation [[Long 2016](https://www.tandfonline.com/doi/abs/10.1080/13658816.2015.1081909?journalCode=tgis20)] |
|                          | Random Trajectory Generator [[Technitis et al. 2015](https://www.tandfonline.com/doi/abs/10.1080/13658816.2014.999682?journalCode=tgis20)] |


## Quick Start
1. Install Docker on your machine see the installation instructions for instance, for [Windows](https://docs.docker.com/docker-for-windows/install/), [Mac](https://docs.docker.com/docker-for-mac/install/) or [Ubuntu Linux](https://docs.docker.com/engine/install/ubuntu/).

2. Run the container

```bash
docker run -p 6013:8888 -p 80:80 -it aniketmitra001/movetk
```

3. Follow instructions in the output of step 2 to open Jupyter Notebook in a web browser. Replace port number 8888 with 6013 (or another port depending on your configuration)


4. Once JupyterLab can be accessed from your browser, in-order to access the notebooks please navigate to ```/tutorials ```  using the navigation pane in JupyterLab 

**Notes** : 

- On Mac it is important to start Docker from Docker Desktop , so that ```localhost``` of the container gets mapped to the ```localhost``` of the host machine. This will allow the documentation to be rendered in Jupyter notebooks.  
- If you are running a ```docker build``` yourself, it it important to ensure that at least 4GB of memory has been allocated to the docker container 


##  Build Artifacts 

A push event to the master branch of MoveTK triggers a set of continuous integration (CI) [workflows](https://github.com/heremaps/movetk/actions). If the worflows are sucessful, this results in the following artifacts to be generated

- A Docker image. Do ```docker pull aniketmitra001/movetk:latest``` to get the image
- A RPM & DEB package. If the latest run of the CI workflow for [MoveTK-CI-Ubuntu](https://github.com/heremaps/movetk/actions?query=workflow%3AMoveTK-CI-Ubuntu+branch%3Amaster) is successful, select the last run. The packages can be found on the *Artifacts* section.   

## Installation with cmake

1. Clone the repository.
```bash
git clone https://github.com/heremaps/movetk.git
```

2. Replicate the  steps in corresponding YAML files

    -[Mac](https://github.com/heremaps/movetk/blob/master/.github/workflows/build-macos.yml)

    -[Windows](https://github.com/heremaps/movetk/blob/master/.github/workflows/build-windows.yml)

    -[Ubuntu](https://github.com/heremaps/movetk/blob/master/.github/workflows/build-ubuntu.yml)

## Using MoveTK in your App

Please refer to this [example](https://github.com/aniketmitra001/movetk-app-template) for a template on how to use MoveTK in your app 

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
