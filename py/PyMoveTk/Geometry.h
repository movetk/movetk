#ifndef PYMOVETK_GEOMETRY_H
#define PYMOVETK_GEOMETRY_H
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"


namespace PyMoveTk {
namespace detail {
/**
 * \brief Wrapper for polyline to avoid list copies from Python.
 * TODO: use pybind11 MAKE_OPAQUE with proper SFINAE for kernel
 * \tparam MovetkPoint
 */
template <typename MovetkPoint>
struct Polyline {
	std::vector<MovetkPoint> points;

	using const_iterator = std::decay_t<decltype(points.cbegin())>;
	using iterator = std::decay_t<decltype(points.begin())>;

	void reserve(std::size_t capacity) { points.reserve(capacity); }
	auto cbegin() const -> decltype(points.cbegin()) { return points.cbegin(); }
	auto cend() const -> decltype(points.cend()) { return points.cend(); }
	auto begin() const { return points.begin(); }
	auto end() const { return points.end(); }
	auto begin() { return points.begin(); }
	auto end() { return points.end(); }
	auto size() const { return points.size(); }
	void push_back(const MovetkPoint &point) { points.push_back(point); }
};
}  // namespace detail

template <typename MovetkGeometryKernel, typename Norm>
struct GeometryModule {
	using NT = typename MovetkGeometryKernel::NT;
	using Segment = typename MovetkGeometryKernel::MovetkSegment;
	using Point = typename MovetkGeometryKernel::MovetkPoint;
	using Vector = typename MovetkGeometryKernel::MovetkVector;
	using Polygon = typename MovetkGeometryKernel::MovetkPolygon;
	using Sphere = typename MovetkGeometryKernel::MovetkSphere;
	using Polyline = detail::Polyline<Point>;
	using Wedge = typename movetk::geom::Wedge<MovetkGeometryKernel, Norm>;
	using CoordinateIterator = movetk::utils::movetk_basic_iterator<const NT>;

	static void register_point(pybind11::module &mod) {
		namespace py = pybind11;

		constexpr const char *NAME = "Point";
		py::class_<Point>(mod, NAME, py::buffer_protocol())
		    /**
		     * Define constructor accepting a buffer via the buffer protocol.
		     * @param buf numpy array as a buffer
		     * @return A  MovetkPoint
		     */
		    .def(py::init([](py::array_t<NT, MovetkGeometryKernel::dim> const buf) {
			    py::buffer_info info = buf.request();
			    auto *first = static_cast<const NT *>(info.ptr);
			    auto *beyond = static_cast<const NT *>(info.ptr) + MovetkGeometryKernel::dim;
			    std::size_t size = std::distance(first, beyond);
			    if (size != MovetkGeometryKernel::dim)
				    throw std::runtime_error("Size of input does not satisfy the predefined dimension of MovetkPoint!");
			    Point pt(first, beyond);
			    return pt;
		    }))
		    /**
		     * @param  m A MovetkPoint
		     * @return dimension of the point
		     */
		    .def("__len__", [](const Point &m) -> std::size_t { return MovetkGeometryKernel::dim; })
		    /**
		     * @param m A MovetkPoint
		     * @param idx index of the point
		     * @return element at the corresponding index of the point
		     */
		    .def("__getitem__",
		         [](const Point &m, std::size_t idx) -> NT {
			         if (idx > MovetkGeometryKernel::dim)
				         throw std::runtime_error("Index out of bounds!");
			         return m[idx];
		         })

		    /**
		     * @brief
		     *
		     */
		    //.def(py::self + py::self)
		    .def(py::self - py::self)
		    //.def(py::self += py::self)
		    //.def(py::self -= py::self)
		    //.def(-py::self)
		    //.def(py::self * NT())
		    //.def(py::self *= NT())
		    //.def(NT() * py::self)
		    ;
	}

	static void register_segment(pybind11::module &mod) {
		constexpr const char *NAME = "Segment";
		namespace py = pybind11;
		py::class_<Segment>(mod, NAME)
		    /**
		     * @brief
		     *
		     */
		    .def(py::init<Point, Point>())
		    /**
		     * @brief
		     *
		     */
		    .def("length",
		         [](const Segment &m) -> NT {
			         movetk::geom::ComputeLength<MovetkGeometryKernel> compute_length;
			         return compute_length(m);
		         })
		    .def("distance",
		         [](Segment &m, Point &p) {
			         movetk::metric::ComputeSquaredDistance<MovetkGeometryKernel, typename MovetkGeometryKernel::Norm>
			             squared_dist;
			         NT dist = squared_dist(p, m);
			         return std::sqrt(dist);
		         })
		    /**
		     * @brief
		     * @param m
		     * @param idx
		     */
		    .def("__getitem__",
		         [](const typename MovetkGeometryKernel::MovetkSegment &m, std::size_t idx) ->
		         typename MovetkGeometryKernel::MovetkPoint {
			         if (idx > 2)
				         throw std::out_of_range("Index out of bounds!");
			         return m[idx];
		         });
	}

	static void register_vector(pybind11::module &mod) {
		namespace py = pybind11;
		/**
		 * @brief
		 *
		 */
		py::class_<Vector>(mod, "Vector")
		    .def(py::init<>())
		    .def("norm",
		         [](Vector &v) -> NT {
			         typename MovetkGeometryKernel::Norm norm;
			         norm(v);
			         NT result = norm ^ 1;
			         return result;
		         })
		    /**
		     * @param  m A MovetkPoint
		     * @return dimension of the point
		     */
		    .def("__len__", [](const Vector &m) -> std::size_t { return MovetkGeometryKernel::dim; })
		    /**
		     * @param m A MovetkPoint
		     * @param idx index of the point
		     * @return element at the corresponding index of the point
		     */
		    .def("__getitem__",
		         [](const Vector &m, std::size_t idx) -> NT {
			         if (idx > MovetkGeometryKernel::dim)
				         throw std::runtime_error("Index out of bounds!");
			         return *std::next(m.begin(), idx);
		         })
		    // Operators
		    .def(py::self *= NT())
		    .def(py::self * NT())
		    .def(py::self / NT())
		    .def(py::self /= NT())
		    // Inner product
		    .def(py::self * py::self)
		    .def(py::self - py::self)
		    .def(py::self + py::self)
		    //.def(-py::self)
		    ;
	}

	static void register_polyline(pybind11::module &mod) {
		namespace py = pybind11;
		constexpr const char *NAME = "Polyline";
		py::class_<Polyline>(mod, NAME)
		    /**
		     * @brief
		     *
		     */
		    .def(py::init<>())
		    .def(py::init<>([](py::array_t<NT> buf) {
			    py::buffer_info info = buf.request();
			    auto access = buf.template unchecked<2>();
			    // TODO: arbitrary dimension, maybe allow transposing.
			    if (access.shape(1) != 2) {
				    throw std::invalid_argument("Expected dimension 1 of array to be of size 2");
			    }
			    Polyline pl;
			    NT coords[2]{0, 0};
			    for (py::ssize_t i = 0; i < access.shape(0); ++i) {
				    for (py::ssize_t j = 0; j < 2; ++j) {
					    coords[j] = access(i, j);
				    }
				    pl.points.emplace_back(&coords[0], &coords[0] + 2);
			    }
			    return pl;
		    }))
		    .def("__getitem__",
		         [](const Polyline &m, std::size_t idx) -> Point {
			         if (idx > m.size())
				         throw std::range_error("Index out of bounds in polyline!");
			         return m.points[idx];
		         })
		    /**
		     * @brief
		     *
		     */
		    .def("push_back", &Polyline::push_back)
		    /**
		     * @brief
		     * @param v
		     */
		    .def("__len__", &Polyline::size)
		    /**
		     * @brief
		     * @param v
		     */
		    .def(
		        "__iter__",
		        [](Polyline &v) { return py::make_iterator(v.begin(), v.end()); },
		        py::keep_alive<0, 1>());
	}

	static void register_polygon(pybind11::module &mod) {
		namespace py = pybind11;
		constexpr const char *NAME = "Polygon";

		py::class_<Polygon>(mod, NAME, py::buffer_protocol())
		    /**
		     * @brief
		     *
		     */
		    .def(py::init<>())
		    /**
		     * @brief
		     * @param p
		     */
		    .def(py::init([](const Polyline &p) {
			    Polygon poly(p.cbegin(), p.cend());
			    return poly;
		    }))
		    /**
		     * @brief
		     * @param polygon
		     * @param idx
		     */
		    .def("__getitem__",
		         [](Polygon &polygon, std::size_t idx) -> Point {
			         const std::size_t num_vertices = std::distance(polygon.v_begin(), polygon.v_end());
			         if (idx >= num_vertices)
				         throw std::runtime_error("Index out of bounds!");
			         auto it = polygon.v_begin() + idx;
			         Point pt(*it);
			         return pt;
		         })
		    /**
		     * @brief
		     * @param polygon
		     */
		    .def("num_vertices",
		         [](Polygon &polygon) -> std::size_t { return std::distance(polygon.v_begin(), polygon.v_end()); });
	}

	static void register_sphere(pybind11::module &mod) {
		namespace py = pybind11;
		constexpr const char *NAME = "Sphere";
		/**
		 * @brief
		 *
		 */
		py::class_<Sphere>(mod, NAME, py::buffer_protocol())
		    /**
		     * @brief
		     *
		     */
		    .def(py::init<>())
		    /**
		     * @brief
		     * @param center
		     * @param radius
		     */
		    .def(py::init([](Point &center, NT radius) {
			    Sphere sphere(center, radius);
			    return sphere;
		    }))
		    .def("radius",
		         [](Sphere &s) -> NT {
			         NT squared_radius = s.squared_radius();
			         return std::sqrt(squared_radius);
		         })
		    .def("center", [](const Sphere &s) { return s.center(); })
		    .def("min_sphere",
		         [](Sphere &s, Polyline &p) -> Sphere {
			         std::vector<NT> vec;
			         movetk::geom::MakeMinSphere<MovetkGeometryKernel> min_sphere;
			         NT radius = min_sphere(p.cbegin(), p.cend(), std::back_inserter(vec));
			         Point center(std::begin(vec), std::end(vec));
			         Sphere sphere(center, radius);
			         s = sphere;
			         return s;
		         })
		    .def("intersection",
		         [](Sphere &sp, Segment &seg) {
			         movetk::geom::ComputeIntersections<typename MovetkGeometryKernel::SphSegIntersectionTraits>
			             compute_sphere_segment_intersections;
			         std::vector<typename MovetkGeometryKernel::SphSegIntersectionTraits::value_type>
			             sphere_segment_intersections;
			         compute_sphere_segment_intersections(sp, seg, std::back_inserter(sphere_segment_intersections));
			         return sphere_segment_intersections;
		         })
		    .def("intersection", [](Sphere &sp1, Sphere &sp2) -> Sphere {
			    movetk::geom::ComputeIntersections<typename MovetkGeometryKernel::SphSegIntersectionTraits>
			        compute_sphere_segment_intersections;
			    typename MovetkGeometryKernel::MovetkSphere sp = compute_sphere_segment_intersections(sp1, sp2);
			    return sp;
		    });
	}
	static void register_wedge(pybind11::module &mod) {
		namespace py = pybind11;
		py::class_<Wedge>(mod, "wedge")
		    .def(py::init<typename MovetkGeometryKernel::MovetkPoint,
		                  typename MovetkGeometryKernel::MovetkPoint,
		                  typename MovetkGeometryKernel::NT>())
		    .def("slope", [](Wedge &w) -> typename MovetkGeometryKernel::MovetkVector { return w.slope(); })
		    .def("intercept", [](Wedge &w) -> typename MovetkGeometryKernel::MovetkVector { return w.intercept(); });
	}

	static void register_module(pybind11::module &mod) {
		namespace py = pybind11;
		register_point(mod);
		register_segment(mod);
		register_vector(mod);
		register_polyline(mod);
		register_polygon(mod);
		register_sphere(mod);
		register_wedge(mod);
	}
};
}  // namespace PyMoveTk

#endif