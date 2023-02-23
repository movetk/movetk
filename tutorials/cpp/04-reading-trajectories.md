# Reading trajectories
MoveTK provides some utilities to read trajectories, which are located in the ``movetk/io`` folder. The main classes to use when reading trajectories are the ``ProbeReader`` and one the the ``TrajectoryReader`` classes.

MoveTK supports reading from ``.csv`` files and compressed versions of CSV files (with ``.gz`` extension). Each measurement of a trajectory is called a ``Probe``. Probes are represented in MoveTK using ``std::tuple<>`` elements. Thus, to be able to work with specific probes, we first need to define the probe type itself. Then, we can define the ``ProbeTraits`` that are needed for MoveTK to understand your probe and be able to read it. For example:
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
enum class ProbeColumns{
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
using DataPoint = 
// Define 
using ProbeCsv = movetk::io::csv::csv<
    ProbePoint,
    Latitude,
    Longitude,
    Time,
    Speed,
    TransportMode>;

static constexpr bool has_header = false;
static constexpr char delimiter = ',';
using ProbeTraits = movetk::io::_ProbeTraits<,,,has_header,delimiter>;
```