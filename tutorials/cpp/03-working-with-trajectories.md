# Working with trajectories
Trajectories are generally represented in MoveTK by a (sorted) range of **probe points** (measurements in the trajectory), where each probe is a tuple that contains spatio-temporal data, as well as any additional data that is associated with the individual probe points. 

## Trajectory Representation
To store such probes, MoveTK provides the ``movetk::ds::TabularTrajectory`` and ``movetk::ds::ColumnarTrajectory`` template classes. These classes differ in the way they store the probes: ``movetk::ds::TabularTrajectory`` stores a list of probe tuples, whereas ``movetk::ds::ColumnarTrajectory`` stores a tuple of lists of the individual  probe fields. For those of you that are familiar with _R_ or Python's _pandas_, these datastructures are similar to the ``DataFrame`` datastructure.
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

### Reading probes
MoveTK supports reading from ``.csv`` files and compressed versions of CSV files (with ``.gz`` extension). Probes are represented in MoveTK using ``std::tuple<>`` elements. Thus, to be able to work with specific probes, we first need to define the probe type itself. Then, we can define the ``ProbeTraits`` that are needed for MoveTK to understand your probe and be able to read it. For example:
```c++
#include <tuple>

enum class TransportMode{
    Train,
    Bus,
    Car
};
// Define your probe type
using ProbePoint = std::tuple<double, double, double, double, TransportMode,std::string>
// Define a description for your columns (for your own convenience)
enum ProbeColumns{
    Latitude = 0,
    Longitude,
    Time,
    Speed,
    TransportMode,
    Id
};
```
Next, we need to define the csv reader. For this, we need to know the data format and the subset of fields that we want to select.
```c++
#include <movetk/io/csv/csv.h>
// Suppose our input has some extra data at the end
using DataPoint = std::tuple<double, double, double, double, TransportMode, std::string, double, double>
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
    TransportMode,
    Id>;

// For our convenience, the resulting tuple is also defined via CsvReader:
static_assert(std::is_same_v<ProbePoint, typename CsvReader::value_type>);
```

Finally, we can now define the traits needed for the ``ProbeReader`` and then define our probe reader:

```c++
#include <movetk/io/ProbeTraits.h>
// Does our file have a header
static constexpr bool has_header = false;
// The delimiter in the file that separates fields
static constexpr char delimiter = ',';
// Define the traits for the reader
using ProbeTraits = movetk::io::_ProbeTraits<ProbeColumns,CsvReader,ProbePoint,has_header,delimiter>;
```

Then, we can construct our ``ProbeReader``.
```c++
#include <movetk/io/ProbeReader.h>
std::string probe_data = "...";
// The probe reader
std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
// Construct the probe reader, using the text as input
probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(probe_data.c_str());
// Or, construct it from a file instead
std::string file_name = "...";
probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(file_name);
```
The ``ProbeReader`` supports ``begin()`` and ``end()`` and thus can be iterated over, resulting in the read probes:
```c++
for (const auto& probe: *probe_reader){
    // Do something with the probe
}
```

### Reading trajectories
We generally want to convert the read probes into trajectories. If each file contains a single trajectory, sorted by time, then simply storing the read trajectories in a ``std::vector`` or ``movetk::ds::(Columnar/Tabular)Trajectory`` should suffice. However, it may be the case that probes of multiple trajectories are present in the same file, and the probes may not be sorted by timestamp. For this usecase, MoveTK provides ``TrajectoryReader`` classes.

To setup the trajectory reader, we need to defined a ``TrajectoryTraits`` class. For this, we first define a trajectory type to store the trajectory in, which can be one of the ``movetk::ds::(Columnar/Tabular)Trajectory`` datastructures. We may use the convenience typedefs ``movetk::ds::(Columnar/Tabular)TrajectoryForProbeType`` to do this. Then, we define the indices of the columns to split and sort by, or -1 if we don't want to split or sort. In the following example, we split by the ``Id`` column and sort the trajectories by the ``Time`` column.
```c++
using Trajectory = movetk::ds::TabularTrajectoryForProbeType<ProbePoint>;
static constexpr auto SplitByFieldIdx = ProbeColumns::Id;
static constexpr int SortByFieldIdx = ProbeColumns::Time;
using TrajectoryTraits =
    movetk::io::_TrajectoryTraits<ProbeTraits, SplitByFieldIdx, SortByFieldIdx, tabular_trajectory_type>;
```
Finally, we can now construct a ``TrajectoryReader`` to read the trajectories using as input the ``ProbeReader`` range:
```c++
using ProbeInputIterator = typename ProbeTraits::ProbeInputIterator;
movetk::io::TrajectoryReader<TrajectoryTraits, ProbeInputIterator> trajectory_reader(probe_reader->begin(), probe_reader->end());
for (const auto& trajectory : trajectory_reader){
    // Do something with our trajectory
}
```

### Custom probe field types
MoveTK supports two additional constructs that may be helpful when parsing your probes: the ``movetk::io::ParseDate`` structure and the ``movetk::io::CategoricalField`` field. 

The ``ParseDate`` structure allows you to specify a datetime format in its constructor. To use your own format, inherit from the ``ParseDate``
```c++
#include <movetk/io/ParseDate.h>
class MyDateTime : public ParseDate{
public:
    MyDateTime(std::time_t timestamp=0): ParseDate(timestamp,"%Y/%m/%d %H:%M:%S"){}
}
```
You can then use this class in the ``std::tuple<>`` defining your probe point and read the timestamp in the provided format. ``ParseDate`` then exposes the timestamp using the ``ts()`` member function.

The ``CategorialField`` allows you to define a categorical field with number of possible values. Internally, it assigns an integer ID to each value. To use it, we need to inherit from it to get unique IDs for our fields:
```c++
#include <movetk/io/CategorialField.h>
struct MyCategory: public movetk::io::CategoricalField<std::string, MyCategory>{};
```
The first template argument here defines what we will read, the second one makes sure that we get unique IDs. Each unique value then gets assigned an ID, which can be retrieved via the ``idx()`` member function on the field.

If you want to use your own type, you have to make sure that the stream operator ``std::istream& operator>>(std::istream& stream, YourType&)`` is defined, and that your type is default constructible.