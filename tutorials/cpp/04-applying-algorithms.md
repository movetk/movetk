# Applying algorithms to your data
Now that we have our ``GeometryKernel`` defined and have read trajectories or created some geometry, we can applies some algorithms to achieve new results. The algorithms in MoveTK are generally defined as function objects: the object can accept template parameters and set internal parameters such as thresholds or maximum number of iterations. To apply the algorithm, the function object implements the call operator ``operator()`` that acts on the element(s) to process.

## Processing trajectories

### Algorithm interface
The algorithms in MoveTK for processing trajectories generally accept pairs of iterators, where specific assumptions are made on the iterators to convey what type they should point to. 

For example, looking at the greedy outlier detection algorithm, we have the following call operator signature
```c++
template <std::random_access_iterator InputIterator, utils::OutputIterator<InputIterator> OutputIterator>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result);
```
Here, the input iterators befine a range of values, and the concept on the ``InputIterator`` specifies that we require it to satisfy the ``random_access_iterator`` concept (e.g. iterators of ``std::vector<>`` or pointers satsify this concept). As is often the case when multiple elements are output, an ``OutputIterator`` is specified, to which the algorithm writes the results. From the concept, we see that this output iterator should accept input iterators.

MoveTK defines more concepts, such as ``movetk::utils::PointIterator<Kernel>`` that requires that the iterator is an iterator over points.