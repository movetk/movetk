# Getting Started

...
## Notes on the tutorial examples
For reading convenience, the examples are provided without a function wrapping the code. The required includes are always at the top of the provided snippets, but should be included outside any used function.

## Selecting a backend and Kernel

MoveTk is designed to be agnostic of the underlying implementation of the geometry. For this, an adapter layer is introduced that defines the interface that we require for the geometry. The geometric objects that MoveTK uses are defined in a ``GeometryKernel``, a collection of type definitions of all the geometric objects. This is similar to the CGAL Kernels (see [link](https://doc.cgal.org/latest/Manual/devman_kernels.html)). MoveTK then makes heavy use of C++ templates, using the geometric object types defined in the ``GeometryKernel`` parameter such that different backends can be chosen.

Out-of-the-box, MoveTK has two backends: one based on ``Boost`` and one on ``CGAL``. When starting an application, the first thing we want to do is define our ``GeometryKernel``.
```c++
// We first pick our desired number type (NT) and the dimensions to support in the Kernel.
// Currently, most of the library only supports 2D and 3D kernels
static constexpr size_t DIMENSIONS = 2;
using NT = long double;

// For the Boost backend
#include<movetk/backends/boost/BoostGeometryWrapper.h>
using GeometryKernel = movetk::backends::boost::KernelFor<NT, dimensions>;
...
// For the CGAL backend
#include<movetk/backends/cgal/CGALGeometryWrapper.h>
using GeometryKernel = movetk::backends::cgal::KernelFor<NT, dimensions>;
```
For most of the constructions and algorithms in MoveTK, the ``GeometryKernel`` is going to be one of the template parameter we need to supply.