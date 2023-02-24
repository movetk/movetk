# Working with trajectories
Trajectories are generally represented in MoveTK by a (sorted) range of **probe points** (measurements in the trajectory), where each probe is a tuple that contains spatio-temporal data, as well as any additional data that is associated with the individual probe points. 

## Trajectory Representation
To store such probes, MoveTK provides the ``movetk::io::TabularTrajectory`` and ``movetk::io::ColumnarTrajectory`` template classes. These classes differ in the way they store the probes: ``movetk::io::TabularTrajectory`` stores a list of probe tuples, whereas ``movetk::io::ColumnarTrajectory`` stores a tuple of lists of the individual  probe fields. For those of you that are familiar with _R_ or Python's _pandas_, these datastructures are similar to the ``DataFrame`` datastructure.
The following example shows how to construct and use these datastructures:
```c++
#include <movetk/ds/ColumnarTrajectory.h>
#include <cassert>
// Lets define a trajectory for x,y,t.
using ColTrajectory = movetk::ds::ColumnarTrajectory<double,double,double>;
std::vector<double> xs{0.0, 3.0,  4.0,  2.0, -1.0};
std::vector<double> ys{2.0, 1.5, -0.5,  0.0, -1.0};
std::vector<double> ts{0.0, 1.0,  2.0,  3.0,  4.0};
ColTrajectory trajectory(std::make_tuple(xs,ys,ts));
assert(trajectory.size() == 5);
static_assert(trajectory.num_fields() == 3);

std::vector<double> fuel{40.0, 35.0, 32.0, 28.0, 20.0};
// Easily add a new column, the new trajectory is 
// now has type movetk::ds::ColumnarTrajectory<double,double,double, double>
auto extended_trajectory = movetk::ds::concat_field(trajectory, fuel);
...
#include <movetk/ds/TabularTrajectory.h>
// Altenatively, if we more easily have access to points, we can use
// the tabular trajectory.

// Define x,y,t points
std::vector<std::tuple<double,double,double>> points{
    std::make_tuple( 0.0,  2.0, 0.0),
    std::make_tuple( 3.0,  1.5, 1.0),
    std::make_tuple( 4.0, -0.5, 2.0),
    std::make_tuple( 2.0,  0.0, 3.0),
    std::make_tuple(-1.0, -1.0, 4.0)
    };
using TabTrajectory = movetk::ds::TabularTrajectory<double,double,double>;
TabTrajectory trajectory(points);
assert(trajectory.size() == 5);
static_assert(trajectory.num_fields() == 3);
```


## Reading trajectories from CSV
MoveTK provides some utilities to read trajectories, which are located in the ``movetk/io`` folder. The main classes to use when reading trajectories are the ``ProbeReader`` and one the the ``TrajectoryReader`` classes.

MoveTK supports reading from ``.csv`` files and compressed versions of CSV files (with ``.gz`` extension). Probes are represented in MoveTK using ``std::tuple<>`` elements. Thus, to be able to work with specific probes, we first need to define the probe type itself. Then, we can define the ``ProbeTraits`` that are needed for MoveTK to understand your probe and be able to read it. For example:
```c++
#include <tuple>

enum class TransportMode{
    Train,
    Bus,
    Car
};
// Define your probe type
using ProbePoint = std::tuple<double, double, double, double, TransportMode>
// Define a description for your columns (for your own convenience)
enum ProbeColumns{
    Latitude = 0,
    Longitude,
    Time,
    Speed,
    TransportMode
};
```
Next, we need to define the ``ProbeTraits`` for the reader to understand how to read the tuple from the CSV
```c++
#include <movetk/io/ProbeTraits.h>
#include 
// Suppose our input has some extra points at the end
using DataPoint = std::tuple<double, double, double, double, TransportMode, double, double>
// Define the CSV reader, only selecting the fields that we want.
// Note that the provided indices (enum values) are indices into the datapoint here.
// The resulting tuple is the subset of the DataPoint tuple, with the indices selecting
// the columns to return.
using CsvReader = movetk::io::csv::csv<
    DataPoint,
    Latitude,
    Longitude,
    Time,
    Speed,
    TransportMode>;

// For our convenience, the resulting tuple is also defined via CsvReader:
static_assert(std::is_same_v<ProbePoint, typename CsvReader::value_type>);

static constexpr bool has_header = false;
static constexpr char delimiter = ',';
using ProbeTraits = movetk::io::_ProbeTraits<,,,has_header,delimiter>;
```