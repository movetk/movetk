# On the design of a geometry backend agnostic library

## Design Goals
**MoveTK** relies on a geometric library.
A number of standard open source libraries are available. The most well known
among them being **CGAL**, **Boost.Geometry** and **GDAL**.
However, each of these libraries differ on in some aspects: 

* **Different Interfaces**: each library has its own interface for construction of geometric objects (points, lines, segments etc.).
* **Coverage of functionality**: these libraries like CGAL provide a comprehensive set of geometric algorithms on these geometric types.
* **Dimension Agnostic**:
* **Licensing Agreements**: Each of these geometric libraries have licences that range from
open access to restrictive access.

Similarly, the choice of the geometry backend also depends on the complexity of the usage and requirements on the robustness. For example, for basic geometric and geographic manipulation, a library like GDAL / Boost.Geometry can suffice.  In constrast, when requiring strong robustness or advanced geometric datastructures, a library like CGAL would be better
choice.

Therefore, one of the design decisions of **MoveTK**
is to let the user choose their own geometry backend. This in turn requires designing a generic interface for different geometric objects. 

[<img src="geomety_agnostic_backend_main_design.jpeg" height="370" width="570"/>](geomety_agnostic_backend_main_design.jpeg)

## Design Patterns

## The Interface
Text.
## The CGAL Adapter
More text.
* \cite HHKPS01 \cite Fabri2000OnTD
*
Even more text \cite BB2009 \cite BBM2010