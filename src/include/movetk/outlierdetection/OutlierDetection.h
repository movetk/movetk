#ifndef MOVETK_OUTLIERDETECTION_OUTLIERDETECTION_H
#define MOVETK_OUTLIERDETECTION_OUTLIERDETECTION_H
namespace movetk::outlierdetection {


/*!
 * @struct OutlierDetectionTraits
 * @brief This traits class serves as a collection of
 * types required by various outlier detection algorithms
 * @tparam _ProbeTraits - This class is a collection of types
 * required to define a Probe. For example @refitem _ProbeTraits
 * @tparam _GeometryTraits - This class is a collection of movetk
 * geometry types. For example @refitem movetk::geom::MovetkGeometryKernel
 * @tparam _Norm - The type that models Euclidean distance
 * For example @refitem movetk_support::FiniteNorm
 * @tparam _ContainerTraits - This class models the data structure used
 * by the outlier detection algorithnms
 */
template <class _ProbeTraits, class _GeometryTraits, class _Norm, class _ContainerTraits = void>
struct OutlierDetectionTraits {
	typedef _Norm Norm;
	typedef _ProbeTraits ProbeTraits;
	typedef _GeometryTraits GeometryTraits;
	typedef _ContainerTraits ContainerTraits;
	typedef typename _GeometryTraits::NT NT;
	typedef typename _GeometryTraits::MovetkPoint Point;
	typedef typename _GeometryTraits::MovetkVector Vector;
};


// Stub for outlier detection algorithms
template <typename GeometryKernel, typename Predicate, typename AlgorithmTag>
class OutlierDetection {};
}  // namespace movetk::outlierdetection
#endif