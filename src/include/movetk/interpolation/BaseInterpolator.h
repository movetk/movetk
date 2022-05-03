#ifndef MOVETK_INTERPOLATION_BASEINTERPOLATOR_H
#define MOVETK_INTERPOLATION_BASEINTERPOLATOR_H
namespace movetk::interpolation {

template <class AlgorithmTag,
          class InterpolationTraits,
          int LatIdx,
          int LonIdx,
          int TsIdx,
          int SpeedIdx = -1,
          int HeadingIdx = -1>
class Interpolator {};
}  // namespace movetk::interpolation
#endif