# MoveTK library design goals

## Geometry backend agnostic
**MoveTK** is setup to make as little as possible assumptions on the implementation of geometric objects and the number types used to represent coordinates. Instead, **MoveTK** defines a geometry interface that is assumed to be present. Then, different libraries can provide an implementation of this interface and thus be used with the **MoveTK** algorithms.

For selecting a library for the geometry, a number of standard open source libraries are available to fulfill this role. The most well known
among them are **CGAL**, **Boost.Geometry** and **GDAL**.
However, each of these libraries differ in some aspects: 

* **Different Interfaces**: each library has its own interface for construction of geometric objects (points, lines, segments etc.).
* **Coverage of functionality**: libraries like CGAL provide a comprehensive set of geometric algorithms on geometric types.
* **Dimension Agnosticity**: some libraries make assumptions on the (maximum) number of dimensions of the geometric objects.
* **Licensing Agreements**: each of these geometric libraries have licences that range from
open access to restrictive access.

Similarly, the choice of the geometry backend also depends on the complexity of the usage and requirements on the robustness. For example, for basic geometric and geographic manipulation, a library like GDAL / Boost.Geometry can suffice.  In constrast, when requiring strong robustness or advanced geometric datastructures, a library like CGAL would be better choice.

![MoveTK structure](movetk_structure.png){html: width=800}

![Overview of the agnostic backend design](geomety_agnostic_backend_main_design.jpeg){html: width=570}

## Extensible 
MoveTK aims to be extensible. It does this in two ways: by providing the backend agnosticism and by following the policy-based design pattern for algorithms. 

## Scalable algorithms
Another design goal of MoveTK is to keep the algorithms as scalable as possible. For this, MoveTK makes use of state-of-the-art algorithms and aims to optimize the implementation as much as possible, while keeping the code maintainable. 

## Ease of use
We strive to make MoveTK as easy to use as possible, while upholding the other design goals. MoveTK aims to provide clear and well-documented interfaces for the algorithms it provides, along with examples, tutorials and tests to further provide the user with the knowledge to start using MoveTK.