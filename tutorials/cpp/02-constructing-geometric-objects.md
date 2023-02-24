# Constructing geometric objects
Now that we have our ``GeometryKernel`` defined, we can construct geometric objects! There are two ways to construct geometric objects: either directly call the constructors of the objects, or use the specialized ``movetk::Make<object>`` functors.
```c++
// Include the following for the creation functors
#include <movetk/geom/ObjectCreation.h>
// Assume we have defined our GeometryKernel
static constexpr size_t DIMENSIONS = 2;
using NT = ...;
using GeometryKernel = ...;

// Create the construction functor
movetk::geom::MakePoint<GeometryKernel> make_point;
// Construct a point. 
// Its type is GeometryKernel::MovetkPoint
auto point = make_point({3.14, 1.618});
// Construct a point using the constructor instead
std::array<NT,2> coords ={-2.14, -3.618};
// Use one of the point constructors instead
GeometryKernel::MovetkPoint point2(coords.begin(), coords.end());

movetk::geom::MakeLine<GeometryKernel> make_line;
// Creates a line through the given points.
auto line = make_line(point, point2);

```
To show what interface the geometric objects provide, MoveTK makes use of C++20 [Concepts](https://en.cppreference.com/w/cpp/language/constraints). Each geometric object has an associated concept in the ``movetk::geom::concepts`` namespace, which are defined in the ``movetk/geom/GeomtryConcepts.h`` file. In addition, the concepts also allow MoveTK to verify that the required interface is implemented by the backend. Here is an example of the ``Point`` concept:
```c++
template <typename POINT, typename KERNEL>
concept Point = requires(POINT& point, const POINT& const_point, size_t index) {
    typename KERNEL::NT;
	typename KERNEL::MovetkVector;
	// Copy and default constructible
	POINT(const_point)
	POINT();

	// Constructible from an initalizer list of coordinates
	POINT(std::declval<std::initializer_list<typename KERNEL::NT>>());
	// Constructible from an input iterator pair. Note that we use std::vector<> iterators as a proxy
	requires requires(typename std::vector<typename KERNEL::NT>::iterator iterator) { POINT(iterator, iterator); };

	// Iteration of coordinates
	const_point.begin();
	const_point.end();

	// Arithmetic
	{ const_point - const_point } -> std::convertible_to<typename KERNEL::MovetkVector>;
};
```
From this, we can see that we can construct a kernel ``Point`` by copying it from another point, by using an initializer list of coordinates or by using a pair of iterators as in the example above. Note that, due to the limitations of concepts, we cannot explicitly define that any input iterator is acceptable for construction. Thus, extra documentation is always provided to make explicit what the semantic meaning of the constraints are.