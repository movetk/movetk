<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>AlgorithmTraits.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/algo/</path>
    <filename>_algorithm_traits_8h</filename>
    <class kind="struct">movetk_algorithms::OutlierDetectionTraits</class>
    <class kind="struct">movetk_algorithms::ClusteringTraits</class>
    <class kind="struct">movetk_algorithms::BBMMParameterTraits</class>
    <namespace>movetk_algorithms</namespace>
  </compound>
  <compound kind="file">
    <name>BrownianBridge.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/algo/</path>
    <filename>_brownian_bridge_8h</filename>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <includes id="_iterators_8h" name="Iterators.h" local="yes" imported="no">movetk/utils/Iterators.h</includes>
    <class kind="struct">movetk_algorithms::brownian_bridge::ParameterTraits</class>
    <class kind="class">movetk_algorithms::brownian_bridge::MLE</class>
    <class kind="class">movetk_algorithms::brownian_bridge::Model</class>
    <class kind="class">movetk_algorithms::brownian_bridge::ParameterSelector</class>
    <class kind="class">movetk_algorithms::brownian_bridge::LogLikelihood</class>
    <namespace>movetk_algorithms</namespace>
    <namespace>movetk_algorithms::brownian_bridge</namespace>
  </compound>
  <compound kind="file">
    <name>Search.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/algo/</path>
    <filename>_search_8h</filename>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <class kind="class">movetk_algorithms::BinarySearch</class>
    <namespace>movetk_algorithms</namespace>
  </compound>
  <compound kind="file">
    <name>Segmentation.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/algo/</path>
    <filename>_segmentation_8h</filename>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <includes id="_iterators_8h" name="Iterators.h" local="yes" imported="no">movetk/utils/Iterators.h</includes>
    <includes id="_search_8h" name="Search.h" local="yes" imported="no">movetk/algo/Search.h</includes>
    <class kind="class">movetk_algorithms::MonotoneSegmentation</class>
    <class kind="class">movetk_algorithms::ModelBasedSegmentation</class>
    <namespace>movetk_algorithms</namespace>
  </compound>
  <compound kind="file">
    <name>SegmentationPredicates.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/algo/</path>
    <filename>_segmentation_predicates_8h</filename>
    <includes id="_trajectory_utils_8h" name="TrajectoryUtils.h" local="yes" imported="no">movetk/utils/TrajectoryUtils.h</includes>
    <class kind="class">movetk_algorithms::TEST</class>
    <class kind="class">movetk_algorithms::TEST&lt;::TestCriteria::meb, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TEST&lt;::TestCriteria::ratio, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TEST&lt;::TestCriteria::difference, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TEST&lt; TestCriteria::range, GeometryTraits &gt;</class>
    <namespace>movetk_algorithms</namespace>
  </compound>
  <compound kind="file">
    <name>SegmentationTraits.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/algo/</path>
    <filename>_segmentation_traits_8h</filename>
    <includes id="_segmentation_predicates_8h" name="SegmentationPredicates.h" local="yes" imported="no">SegmentationPredicates.h</includes>
    <includes id="_segmentation_8h" name="Segmentation.h" local="yes" imported="no">Segmentation.h</includes>
    <class kind="struct">movetk_algorithms::SegmentationTraits</class>
    <namespace>movetk_algorithms</namespace>
  </compound>
  <compound kind="file">
    <name>StartStopMatrix.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/ds/</path>
    <filename>_start_stop_matrix_8h</filename>
    <includes id="_iterators_8h" name="Iterators.h" local="yes" imported="no">movetk/utils/Iterators.h</includes>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <class kind="class">movetk_support::StartStopDiagram</class>
    <class kind="class">movetk_support::StartStopDiagram&lt; SsdType::compressed, GeometryTraits, NodeType &gt;</class>
    <namespace>movetk_support</namespace>
  </compound>
  <compound kind="file">
    <name>Tree.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/ds/</path>
    <filename>_tree_8h</filename>
    <includes id="_asserts_8h" name="Asserts.h" local="yes" imported="no">movetk/utils/Asserts.h</includes>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <class kind="class">movetk_support::TrieNode</class>
    <class kind="class">movetk_support::BinaryNode</class>
    <class kind="class">movetk_support::Tree</class>
    <namespace>movetk_support</namespace>
    <member kind="function">
      <type>char</type>
      <name>f</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a75f9b88e76f5b6cf92fa3a53d221d1e0</anchor>
      <arglist>(char &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>const char</type>
      <name>f</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a57d547ec1ca58852160c9d507db8c9b7</anchor>
      <arglist>(const char &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>f</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a5acb93f515a01bd69661b5c0dcf0bc9a</anchor>
      <arglist>(int &amp;v)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>CGALTraits.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/geom/</path>
    <filename>_c_g_a_l_traits_8h</filename>
    <class kind="struct">movetk_support::CGALTraits</class>
    <namespace>movetk_support</namespace>
  </compound>
  <compound kind="file">
    <name>Asserts.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/utils/</path>
    <filename>_asserts_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_RANDOM_ACCESS_ITERATOR</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>a2873ad022f1d3c982067b154ff1bb506</anchor>
      <arglist>(IteratorType)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_FORWARD_ITERATOR</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>aa282f918bf146435fff98a704f9119f5</anchor>
      <arglist>(IteratorType)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_OUTPUT_ITERATOR</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>a789fbdaef198779e661baae882bc9311</anchor>
      <arglist>(IteratorType)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_NUMBER_TYPE</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>a18ae2fb3a88b11fa773b9d38f971b0a5</anchor>
      <arglist>(Kernel, Iterator)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_WRAPPER_POINT_TYPE</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>ad15beb9b86ca7c0a669238ef6d4c459a</anchor>
      <arglist>(Kernel, Iterator)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_MOVETK_POINT_TYPE</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>a5823d71b00e6dcc5d6b725aeb6ce0193</anchor>
      <arglist>(Kernel, Iterator)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_IS_STRING</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>a9f3d86aa3ef178bf0b5a8da309229359</anchor>
      <arglist>(IteratorType)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERT_IS_PAIR</name>
      <anchorfile>_asserts_8h.html</anchorfile>
      <anchor>aa4f747ba2a0866c63d9cfa4cb19de8b0</anchor>
      <arglist>(IteratorType)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Iterators.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/utils/</path>
    <filename>_iterators_8h</filename>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <class kind="class">movetk_core::movetk_back_insert_iterator</class>
    <class kind="class">movetk_core::movetk_insert_iterator</class>
    <class kind="class">movetk_core::movetk_basic_iterator</class>
    <class kind="class">movetk_core::movetk_grid_iterator</class>
    <namespace>movetk_core</namespace>
  </compound>
  <compound kind="file">
    <name>Requirements.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/utils/</path>
    <filename>_requirements_8h</filename>
    <namespace>movetk_core</namespace>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_size_t&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_size_t</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a5192129fe513a04df1806227c5f925b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_date&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_date_t</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a86945ffb9bd9cadc53b5bc4217209bce</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_NT&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_NT</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a314e8a963a2e98758edb872b67d1fd1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_random_access_iterator&lt; typename std::iterator_traits&lt; T &gt;::iterator_category &gt;::value, T &gt;::type</type>
      <name>requires_random_access_iterator</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a228aeece4617967d07f415e1afb0e12b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_output_iterator&lt; typename std::iterator_traits&lt; T &gt;::iterator_category &gt;::value, T &gt;::type</type>
      <name>requires_output_iterator</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a29421896067c68d715944445897b221b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkPoint&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_movetk_point</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a3419c3fc687241dade3c82db464c44e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkSegment&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_movetk_segment</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>aaab915fb8b6ff511427bff2acca7695d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkLine&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_movetk_Line</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>ab0b4a19a63d12a6a2f21a760f15f2d8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_Wrapper_Point&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_wrapper_point</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a65016293df2e312c04fb70c205b76da5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_pair&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_pair</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a3a66cac37ab51fd7df229337566c5f1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_equal_to&lt; T1, T2 &gt;::value, T2 &gt;::type</type>
      <name>requires_equality</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a509e9d52c86339907ba5bbc1c4001700</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_NT&lt; Traits, T &gt;::value||movetk_core::is_size_t&lt; T &gt;::value||movetk_core::is_date&lt; ParseDate &gt;::value||movetk_core::is_MovetkPoint&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_valid_type</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>aac05f5bc2d9b45fb47f9184736626cbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_string&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_string</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a32472f7f6df250cc350b7b6849adccc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; std::is_arithmetic&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_arithmetic</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a0b467e229a160251d09a861898cfda5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkPoint&lt; Traits, T &gt;::value||movetk_core::is_MovetkLine&lt; Traits, T &gt;::value||movetk_core::is_MovetkSegment&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_valid_type_distance_computation</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a7dd9abd1739b1fb3f18c712402346e72</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>StringUtils.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/utils/</path>
    <filename>_string_utils_8h</filename>
    <includes id="_asserts_8h" name="Asserts.h" local="yes" imported="no">Asserts.h</includes>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">Requirements.h</includes>
    <class kind="struct">movetk_support::cast</class>
    <class kind="struct">movetk_support::cast&lt; long double &gt;</class>
    <namespace>movetk_support</namespace>
    <member kind="function">
      <type>std::string</type>
      <name>join</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a254e533bd19de6a2448c8deda42829d2</anchor>
      <arglist>(IteratorType first, IteratorType beyond, const char &amp;delim=&apos;,&apos;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>split</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>ac8100ee56459eb96ff1e73ae3485adf8</anchor>
      <arglist>(std::string &amp;in, OutputIterator iter, const char &amp;delim=&apos;,&apos;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>TrajectoryUtils.h</name>
    <path>/Users/amitra/Documents/movetk/src/include/movetk/utils/</path>
    <filename>_trajectory_utils_8h</filename>
    <includes id="_requirements_8h" name="Requirements.h" local="yes" imported="no">movetk/utils/Requirements.h</includes>
    <class kind="class">movetk_core::SegmentIdGenerator</class>
    <namespace>movetk_core</namespace>
    <member kind="function">
      <type>bool</type>
      <name>is_sequence</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>af1f92171c2caf7ec3d88fcbdee331f7e</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>InputIterator</type>
      <name>min_non_zero_element</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>ad7788c6faac2e4e49de71689ddbeb0c8</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_time_diffs</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a9947fcacd7b9df537389e90b553141d0</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_distances</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a1f250e1e7a7b1f0cd03ca68c144f2270</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_speeds</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>abfb27ac2474ab425d277191cda17d453</anchor>
      <arglist>(TDiffIterator TdiffFirst, TDiffIterator TdiffBeyond, DistanceIterator DistFirst, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_headings</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a4ba2ccd9fd0981543c81b85382b18e38</anchor>
      <arglist>(InputIterator first, InputIterator beyond, PointsIterator pfirst, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_velocities</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a561fe757d7de59d12a4ec8180940aa4d</anchor>
      <arglist>(MagnitudeIterator Mfirst, MagnitudeIterator Mbeyond, DirectionIterator DFirst, OutputIterator iter)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>_HighFrequencyTrajectoryTraits</name>
    <filename>struct___high_frequency_trajectory_traits.html</filename>
    <templarg></templarg>
    <templarg>_DateIdx</templarg>
    <templarg>_LatIdx</templarg>
    <templarg>_LonIdx</templarg>
  </compound>
  <compound kind="struct">
    <name>_ProbeTraits</name>
    <filename>struct___probe_traits.html</filename>
    <templarg>_ProbeColumns</templarg>
    <templarg>_ProbeParseDate</templarg>
    <templarg>_ProbeCsv</templarg>
    <templarg>_ProbePoint</templarg>
    <templarg>_header</templarg>
    <templarg>_delimiter</templarg>
  </compound>
  <compound kind="struct">
    <name>_TrajectoryTraits</name>
    <filename>struct___trajectory_traits.html</filename>
    <templarg></templarg>
    <templarg>_SplitByFieldIdx</templarg>
    <templarg>_SortByFieldIdx</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::BBMMParameterTraits</name>
    <filename>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="enumeration">
      <type></type>
      <name>ParameterColumns</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>POINT</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6ea9df00c14439e44e2ab0cd3327db93227</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MU</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6ead3ac49635a85ccd60a0e81d789eaa6e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SIGMA_SQUARED</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6eab66ce4cf7000176d85214a00b4fa1b37</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BEGIN</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6eaeda66125fed486fef1ee482b67ff574c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>END</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6ea5ede3e960a2eaf34046209b8f2a55b6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>GeometryTraits::NT</type>
      <name>NT</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a107fb95bcaf7c4e885b528b4e0815090</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>GeometryTraits::MovetkPoint</type>
      <name>Point</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a81d8167c1375271b5e87f3d42c847272</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>GeometryTraits::MovetkVector</type>
      <name>Vector</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a2b58b976ac38b64ceb2ff4ddfed9282b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::tuple&lt; Point, Point, NT, TrajectoryIterator, TrajectoryIterator &gt;</type>
      <name>Parameters</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a6e1ec765e868524906e0e706b3f3bd34</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>POINT</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6ea9df00c14439e44e2ab0cd3327db93227</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MU</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6ead3ac49635a85ccd60a0e81d789eaa6e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SIGMA_SQUARED</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6eab66ce4cf7000176d85214a00b4fa1b37</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BEGIN</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6eaeda66125fed486fef1ee482b67ff574c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>END</name>
      <anchorfile>structmovetk__algorithms_1_1_b_b_m_m_parameter_traits.html</anchorfile>
      <anchor>a40e6b46abbe598ba218381655f96bd6ea5ede3e960a2eaf34046209b8f2a55b6f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BBMMTraits</name>
    <filename>struct_b_b_m_m_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>BBParameterTraits</name>
    <filename>struct_b_b_parameter_traits.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_support::BinaryNode</name>
    <filename>classmovetk__support_1_1_binary_node.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>BinaryNode</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a3070382dc3790bb8f8aca389b5f0af66</anchor>
      <arglist>(K &amp;InKey)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a60cfc224f605c9c02549eed955ae59b0</anchor>
      <arglist>(NodePtr nd)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a10fe956cbf54a94189e36fc9fc7facaf</anchor>
      <arglist>(K &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a19863a95f35cd9d7d0d696d476af9143</anchor>
      <arglist>(reference nd)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a93b8d592eb36a400797310f67e090f2f</anchor>
      <arglist>(reference nd)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>aba2e62fd96ccda08f4661c4effe5de0f</anchor>
      <arglist>(reference nd)</arglist>
    </member>
    <member kind="function">
      <type>K</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>aa4da235648f2b1e09f0aae153a22a482</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>NodePtr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>ae4dd8042addac80728f1381ca2593882</anchor>
      <arglist>(K &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>NodePtr &amp;</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a0313c33d87f0fa7aeb1eaeb4b492a2c7</anchor>
      <arglist>(iterator iter)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>begin</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>afb8ee766c94a93b452a0d96854e69072</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>end</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>add0abc3273cdac472092d099b0bae193</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert_value</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>ace56fc988da1f0962c77b1afc865731d</anchor>
      <arglist>(std::string &amp;id, T &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>ValuesIterator</type>
      <name>v_begin</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a2d1c7519077c97bcdbe34592cd85159d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ValuesIterator</type>
      <name>v_end</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>a3b0470a7a2a053d04eea6f37d23e3709</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string &amp;</type>
      <name>v_id</name>
      <anchorfile>classmovetk__support_1_1_binary_node.html</anchorfile>
      <anchor>af02946e8bb64ff2b692659f6f531572e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::BinarySearch</name>
    <filename>classmovetk__algorithms_1_1_binary_search.html</filename>
    <templarg>GeometryTraits</templarg>
    <templarg>TestType</templarg>
    <member kind="function">
      <type></type>
      <name>BinarySearch</name>
      <anchorfile>classmovetk__algorithms_1_1_binary_search.html</anchorfile>
      <anchor>af40d53261260406c371a673d459dd59e</anchor>
      <arglist>(NT threshold)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_binary_search.html</anchorfile>
      <anchor>adb880218d0bc52ebbda4a975b52b1811</anchor>
      <arglist>(InputIterator first, size_t min, size_t left, size_t right)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BinarySearch&lt; GeometryTraits, TEST &gt;</name>
    <filename>classmovetk__algorithms_1_1_binary_search.html</filename>
    <member kind="function">
      <type></type>
      <name>BinarySearch</name>
      <anchorfile>classmovetk__algorithms_1_1_binary_search.html</anchorfile>
      <anchor>af40d53261260406c371a673d459dd59e</anchor>
      <arglist>(NT threshold)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_binary_search.html</anchorfile>
      <anchor>adb880218d0bc52ebbda4a975b52b1811</anchor>
      <arglist>(InputIterator first, size_t min, size_t left, size_t right)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_support::BoostGeometryTraits</name>
    <filename>structmovetk__support_1_1_boost_geometry_traits.html</filename>
    <templarg></templarg>
    <templarg>dimensions</templarg>
  </compound>
  <compound kind="struct">
    <name>cartesian_coordinates_tag</name>
    <filename>structcartesian__coordinates__tag.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::cast</name>
    <filename>structmovetk__support_1_1cast.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::cast&lt; long double &gt;</name>
    <filename>structmovetk__support_1_1cast_3_01long_01double_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>CategoricalField</name>
    <filename>struct_categorical_field.html</filename>
    <templarg>T</templarg>
    <templarg>CRTP</templarg>
  </compound>
  <compound kind="class">
    <name>CategoricalField&lt; std::string, ProviderCategoricalField &gt;</name>
    <filename>struct_categorical_field.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::CGALTraits</name>
    <filename>structmovetk__support_1_1_c_g_a_l_traits.html</filename>
    <templarg></templarg>
    <templarg>dimensions</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::ChanChin</name>
    <filename>classmovetk__algorithms_1_1_chan_chin.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::ClusteringTraits</name>
    <filename>structmovetk__algorithms_1_1_clustering_traits.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>ColumnarTrajectory</name>
    <filename>class_columnar_trajectory.html</filename>
    <templarg>fields</templarg>
    <member kind="typedef">
      <type>std::tuple_element_t&lt; FieldIdx, value_type &gt;</type>
      <name>FieldType</name>
      <anchorfile>class_columnar_trajectory.html</anchorfile>
      <anchor>a825f6c3103bce8c616fc30f199c315ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ColumnarTrajectory</name>
      <anchorfile>class_columnar_trajectory.html</anchorfile>
      <anchor>a965bd8e80afb727c61033152c17f6743</anchor>
      <arglist>(std::tuple&lt; std::vector&lt; fields &gt;... &gt; points)</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>size</name>
      <anchorfile>class_columnar_trajectory.html</anchorfile>
      <anchor>a009e6152685b2d08fb39bba6968a15aa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>IteratorTuple</type>
      <name>row_end</name>
      <anchorfile>class_columnar_trajectory.html</anchorfile>
      <anchor>ab5a1d72869a462dfffbb850b194fefea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>IteratorTuple</type>
      <name>row_next</name>
      <anchorfile>class_columnar_trajectory.html</anchorfile>
      <anchor>aa9ce59e6c46defead4ef7c494df6c66a</anchor>
      <arglist>(std::tuple&lt; typename std::vector&lt; fields &gt;::iterator... &gt; &amp;iters)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::ComputeDiscreteFrechetDistance</name>
    <filename>structmovetk__core_1_1_compute_discrete_frechet_distance.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::ComputeDiscreteHausdorffDistance</name>
    <filename>structmovetk__core_1_1_compute_discrete_hausdorff_distance.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_core::ComputeIntersections</name>
    <filename>classmovetk__core_1_1_compute_intersections.html</filename>
    <templarg>IntersectionTraits</templarg>
  </compound>
  <compound kind="class">
    <name>ComputeIntersections&lt; movetk_core::IntersectionTraits &gt;</name>
    <filename>classmovetk__core_1_1_compute_intersections.html</filename>
  </compound>
  <compound kind="class">
    <name>ComputeIntersections&lt; typename CellTraits::IntersectionTraits &gt;</name>
    <filename>classmovetk__core_1_1_compute_intersections.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::ComputeLength</name>
    <filename>structmovetk__core_1_1_compute_length.html</filename>
    <templarg>GeometryTraits</templarg>
    <member kind="function">
      <type>GeometryTraits::NT</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_compute_length.html</anchorfile>
      <anchor>ab5eea74c19032c5663b285243fcabec4</anchor>
      <arglist>(typename GeometryTraits::MovetkPoint p1, typename GeometryTraits::MovetkPoint p2)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::ComputeSquaredDistance</name>
    <filename>structmovetk__core_1_1_compute_squared_distance.html</filename>
    <templarg>GeometryTraits</templarg>
    <templarg>Norm</templarg>
  </compound>
  <compound kind="class">
    <name>boost::geometry::detail::discrete_frechet_distance::coup_mat</name>
    <filename>classboost_1_1geometry_1_1detail_1_1discrete__frechet__distance_1_1coup__mat.html</filename>
    <templarg>size_type1</templarg>
    <templarg>size_type2</templarg>
    <templarg>result_type</templarg>
  </compound>
  <compound kind="class">
    <name>csv</name>
    <filename>classcsv.html</filename>
    <templarg></templarg>
    <templarg>selidx</templarg>
    <class kind="class">csv::iterator</class>
    <member kind="function">
      <type></type>
      <name>csv</name>
      <anchorfile>classcsv.html</anchorfile>
      <anchor>a831f808636d085e17d041bf248c4ddb2</anchor>
      <arglist>(std::istream &amp;in, const char delim, const bool header)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>good</name>
      <anchorfile>classcsv.html</anchorfile>
      <anchor>a6fa9045cd714527e9f24f867e8cd1cc0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>good</name>
      <anchorfile>classcsv.html</anchorfile>
      <anchor>a6fa9045cd714527e9f24f867e8cd1cc0</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CsvProbeReader</name>
    <filename>class_csv_probe_reader.html</filename>
    <templarg></templarg>
    <base>ProbeReader</base>
  </compound>
  <compound kind="class">
    <name>movetk_support::Discrete_Frechet</name>
    <filename>classmovetk__support_1_1_discrete___frechet.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_frechet_distance</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__frechet__distance.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_frechet_distance&lt; Linestring1, Linestring2, linestring_tag, linestring_tag &gt;</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__frechet__distance_3_01_linestring1_00_01_linestb9ef3b08e3e7307ad611fcbb6a758f5a.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <base>boost::geometry::detail::discrete_frechet_distance::linestring_linestring</base>
  </compound>
  <compound kind="struct">
    <name>movetk_core::discrete_frechet_distance_algorithm</name>
    <filename>structmovetk__core_1_1discrete__frechet__distance__algorithm.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::discrete_frechet_distance_algorithm&lt; GeometryTraits, Norm, void &gt;</name>
    <filename>structmovetk__core_1_1discrete__frechet__distance__algorithm_3_01_geometry_traits_00_01_norm_00_01void_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Discrete_Hausdorff</name>
    <filename>classmovetk__support_1_1_discrete___hausdorff.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_hausdorff_distance</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__hausdorff__distance.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_hausdorff_distance&lt; linestring, multi_linestring, linestring_tag, multi_linestring_tag &gt;</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__hausdorff__distance_3_01linestring_00_01multi__a1c323402b2b69577807239a27dbf5df.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <base>boost::geometry::detail::discrete_hausdorff_distance::range_multi_range</base>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_hausdorff_distance&lt; Linestring1, Linestring2, linestring_tag, linestring_tag &gt;</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__hausdorff__distance_3_01_linestring1_00_01_lined2f0ce921655b34284dae1b41dc6baaa.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <base>boost::geometry::detail::discrete_hausdorff_distance::range_range</base>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_hausdorff_distance&lt; multi_linestring1, multi_linestring2, multi_linestring_tag, multi_linestring_tag &gt;</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__hausdorff__distance_3_01multi__linestring1_00_05997563b9ea83fb632c38441e639df97.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <base>boost::geometry::detail::discrete_hausdorff_distance::multi_range_multi_range</base>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_hausdorff_distance&lt; MultiPoint1, MultiPoint2, multi_point_tag, multi_point_tag &gt;</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__hausdorff__distance_3_01_multi_point1_00_01_mulaa51ea5dcfb48c99d0ea1bdf6eb39d99.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <base>boost::geometry::detail::discrete_hausdorff_distance::range_range</base>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::dispatch::discrete_hausdorff_distance&lt; Point, MultiPoint, point_tag, multi_point_tag &gt;</name>
    <filename>structboost_1_1geometry_1_1dispatch_1_1discrete__hausdorff__distance_3_01_point_00_01_multi_poinacdcf7b82b0d663116c72b720d8e2d4e.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <base>boost::geometry::detail::discrete_hausdorff_distance::point_range</base>
  </compound>
  <compound kind="struct">
    <name>movetk_core::discrete_hausdorff_distance_algorithm</name>
    <filename>structmovetk__core_1_1discrete__hausdorff__distance__algorithm.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::discrete_hausdorff_distance_algorithm&lt; GeometryTraits, Norm, void &gt;</name>
    <filename>structmovetk__core_1_1discrete__hausdorff__distance__algorithm_3_01_geometry_traits_00_01_norm_00_01void_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::DouglasPeucker</name>
    <filename>classmovetk__algorithms_1_1_douglas_peucker.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::DummyOffsetInterpolator_2</name>
    <filename>classmovetk__algorithms_1_1_dummy_offset_interpolator__2.html</filename>
    <templarg></templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <templarg>SpeedIdx</templarg>
    <templarg>HeadingIdx</templarg>
    <member kind="function">
      <type>TrajectoryPoint</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_dummy_offset_interpolator__2.html</anchorfile>
      <anchor>a67cda08fb161a7c80ceaadb2b43bfef1</anchor>
      <arglist>(const TrajectoryPoint &amp;p1, const TrajectoryPoint &amp;p2, double offset)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::DynamicTimeWarping</name>
    <filename>classmovetk__algorithms_1_1_dynamic_time_warping.html</filename>
  </compound>
  <compound kind="class">
    <name>TabularTrajectory::FieldIterator</name>
    <filename>class_tabular_trajectory_1_1_field_iterator.html</filename>
    <templarg>field_idx</templarg>
    <member kind="function">
      <type></type>
      <name>FieldIterator</name>
      <anchorfile>class_tabular_trajectory_1_1_field_iterator.html</anchorfile>
      <anchor>a08787c47c51d888daffce2f94694a033</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>FieldIterator</name>
      <anchorfile>class_tabular_trajectory_1_1_field_iterator.html</anchorfile>
      <anchor>ab538ea115d2f176f439122f6dda2c6e6</anchor>
      <arglist>(TabularTrajectory &amp;parent)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::FindFarthest</name>
    <filename>structmovetk__algorithms_1_1_find_farthest.html</filename>
    <templarg>GeometryTraits</templarg>
    <templarg>Norm</templarg>
  </compound>
  <compound kind="class">
    <name>FindFarthest&lt; MovetkGeometryKernel, Norm &gt;</name>
    <filename>structmovetk__algorithms_1_1_find_farthest.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_support::FiniteNorm</name>
    <filename>classmovetk__support_1_1_finite_norm.html</filename>
    <templarg></templarg>
    <templarg>p</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::FreeSpaceCell</name>
    <filename>classmovetk__support_1_1_free_space_cell.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>FreeSpaceCell</name>
      <anchorfile>classmovetk__support_1_1_free_space_cell.html</anchorfile>
      <anchor>a776a51a65ac7a00c964c2e72d8b4159c</anchor>
      <arglist>(typename CellTraits::GeometryTraits::MovetkSegment P, typename CellTraits::GeometryTraits::MovetkSegment Q, typename CellTraits::GeometryTraits::NT radius)</arglist>
    </member>
    <member kind="function">
      <type>CellTraits::vertex_iterator</type>
      <name>vertices_begin</name>
      <anchorfile>classmovetk__support_1_1_free_space_cell.html</anchorfile>
      <anchor>a590f75f7d0fb3401b19fb00480dafc31</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>CellTraits::vertex_iterator</type>
      <name>vertices_end</name>
      <anchorfile>classmovetk__support_1_1_free_space_cell.html</anchorfile>
      <anchor>aa513381e305985e94bd4cd4a2a849e08</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>CellTraits::const_iterator</type>
      <name>begin</name>
      <anchorfile>classmovetk__support_1_1_free_space_cell.html</anchorfile>
      <anchor>ad939264bde0cca90bcce839d243bcfc7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>CellTraits::const_iterator</type>
      <name>end</name>
      <anchorfile>classmovetk__support_1_1_free_space_cell.html</anchorfile>
      <anchor>a79cf519249bbdd2ad7397adefcc9d589</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_support::FreeSpaceCellTraits</name>
    <filename>structmovetk__support_1_1_free_space_cell_traits.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::FreeSpaceDiagram</name>
    <filename>classmovetk__support_1_1_free_space_diagram.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::FreeSpaceDiagramTraits</name>
    <filename>structmovetk__support_1_1_free_space_diagram_traits.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>GdalPolygonReader</name>
    <filename>class_gdal_polygon_reader.html</filename>
  </compound>
  <compound kind="struct">
    <name>geo_coordinates_tag</name>
    <filename>structgeo__coordinates__tag.html</filename>
  </compound>
  <compound kind="struct">
    <name>GeometryKernel</name>
    <filename>struct_geometry_kernel.html</filename>
  </compound>
  <compound kind="struct">
    <name>greedy_outlier_detector_tag</name>
    <filename>structgreedy__outlier__detector__tag.html</filename>
  </compound>
  <compound kind="struct">
    <name>GroupedHighFrequencyTrajectoryReader</name>
    <filename>struct_grouped_high_frequency_trajectory_reader.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>HighFrequencyTrajectoryReader</name>
    <filename>class_high_frequency_trajectory_reader.html</filename>
    <templarg></templarg>
    <templarg>TrajectoriesAreGrouped</templarg>
  </compound>
  <compound kind="class">
    <name>HighFrequencyTrajectorySplitter</name>
    <filename>class_high_frequency_trajectory_splitter.html</filename>
    <templarg></templarg>
    <templarg>DateIdx</templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <class kind="class">HighFrequencyTrajectorySplitter::iterator</class>
    <member kind="function">
      <type>bool</type>
      <name>good</name>
      <anchorfile>class_high_frequency_trajectory_splitter.html</anchorfile>
      <anchor>a042d6cecbc784e45d2f3807723a38429</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::ImaiIri</name>
    <filename>classmovetk__algorithms_1_1_imai_iri.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::InfinityNorm</name>
    <filename>classmovetk__support_1_1_infinity_norm.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::InterpolationTraits</name>
    <filename>structmovetk__algorithms_1_1_interpolation_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::Interpolator</name>
    <filename>classmovetk__algorithms_1_1_interpolator.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <templarg>TsIdx</templarg>
    <templarg>SpeedIdx</templarg>
    <templarg>HeadingIdx</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::Interpolator&lt; movetk_algorithms::kinematic_interpolator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx &gt;</name>
    <filename>classmovetk__algorithms_1_1_interpolator_3_01movetk__algorithms_1_1kinematic__interpolator__tag_9858d5422a7946ad7d6a7ba7c3f83dc7.html</filename>
    <templarg></templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <templarg>TsIdx</templarg>
    <templarg>SpeedIdx</templarg>
    <templarg>HeadingIdx</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::Interpolator&lt; movetk_algorithms::linear_interpolator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx &gt;</name>
    <filename>classmovetk__algorithms_1_1_interpolator_3_01movetk__algorithms_1_1linear__interpolator__tag_00_728b7a8731aa441c4bae05e9769be93a.html</filename>
    <templarg></templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <templarg>TsIdx</templarg>
    <templarg>SpeedIdx</templarg>
    <templarg>HeadingIdx</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::Interpolator&lt; movetk_algorithms::random_trajectory_generator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx &gt;</name>
    <filename>classmovetk__algorithms_1_1_interpolator_3_01movetk__algorithms_1_1random__trajectory__generator017db4952046318daac25e26361b579f.html</filename>
    <templarg></templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <templarg>TsIdx</templarg>
    <templarg>SpeedIdx</templarg>
    <templarg>HeadingIdx</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::CGAL_Algorithms::Intersection_visitor</name>
    <filename>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_intersection__visitor.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>Kernel::IntersectionVariant</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_intersection__visitor.html</anchorfile>
      <anchor>ae2b4274212796e3b297f651d98fbca19</anchor>
      <arglist>(const CGAL_Point &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>Kernel::IntersectionVariant</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_intersection__visitor.html</anchorfile>
      <anchor>a0f36b68569b4bcf99e25e841c802ceaf</anchor>
      <arglist>(const CGAL_Segment &amp;s) const</arglist>
    </member>
    <member kind="function">
      <type>Kernel::IntersectionVariant</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_intersection__visitor.html</anchorfile>
      <anchor>a2411f95f1e5b344d0bbfcddf5d8efca3</anchor>
      <arglist>(const CGAL_Line &amp;l) const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::IntersectionTraits</name>
    <filename>structmovetk__core_1_1_intersection_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::IntersectionTraits&lt; _GeometryTraits, _Norm, polylines_instersection_tag &gt;</name>
    <filename>structmovetk__core_1_1_intersection_traits_3_01___geometry_traits_00_01___norm_00_01polylines__instersection__tag_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::IntersectionTraits&lt; _GeometryTraits, _Norm, sphere_segment_intersection_tag &gt;</name>
    <filename>structmovetk__core_1_1_intersection_traits_3_01___geometry_traits_00_01___norm_00_01sphere__segment__intersection__tag_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::IntersectionTraits&lt; _GeometryTraits, _Norm, sphere_sphere_intersection_tag &gt;</name>
    <filename>structmovetk__core_1_1_intersection_traits_3_01___geometry_traits_00_01___norm_00_01sphere__sphere__intersection__tag_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>IPTraits</name>
    <filename>struct_i_p_traits.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_date</name>
    <filename>structmovetk__core_1_1is__date.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_date&lt; const ParseDate &gt;</name>
    <filename>structmovetk__core_1_1is__date_3_01const_01_parse_date_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_date&lt; ParseDate &gt;</name>
    <filename>structmovetk__core_1_1is__date_3_01_parse_date_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_date&lt; std::size_t &gt;</name>
    <filename>structmovetk__core_1_1is__date_3_01std_1_1size__t_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_date&lt; std::time_t &gt;</name>
    <filename>structmovetk__core_1_1is__date_3_01std_1_1time__t_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_directed_graph</name>
    <filename>structmovetk__core_1_1is__directed__graph.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_directed_graph&lt; boost::directedS &gt;</name>
    <filename>structmovetk__core_1_1is__directed__graph_3_01boost_1_1directed_s_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_equal_to</name>
    <filename>structmovetk__core_1_1is__equal__to.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_equal_to&lt; T1, T1 &gt;</name>
    <filename>structmovetk__core_1_1is__equal__to_3_01_t1_00_01_t1_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_forward_iterator</name>
    <filename>structmovetk__core_1_1is__forward__iterator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_forward_iterator&lt; std::forward_iterator_tag &gt;</name>
    <filename>structmovetk__core_1_1is__forward__iterator_3_01std_1_1forward__iterator__tag_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_L2_norm</name>
    <filename>structmovetk__core_1_1is___l2__norm.html</filename>
    <templarg>p</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_L2_norm&lt; 2 &gt;</name>
    <filename>structmovetk__core_1_1is___l2__norm_3_012_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::is_monotone</name>
    <filename>structmovetk__algorithms_1_1is__monotone.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkLine</name>
    <filename>structmovetk__core_1_1is___movetk_line.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkLine&lt; Kernel, const typename Kernel::MovetkLine &amp; &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_line_3_01_kernel_00_01const_01typename_01_kernel_1_1_movetk_line_01_6_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkLine&lt; Kernel, typename Kernel::MovetkLine &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_line_3_01_kernel_00_01typename_01_kernel_1_1_movetk_line_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkPoint</name>
    <filename>structmovetk__core_1_1is___movetk_point.html</filename>
    <templarg>Kernel</templarg>
    <templarg>Type</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkPoint&lt; Kernel, const typename Kernel::MovetkPoint &amp; &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_point_3_01_kernel_00_01const_01typename_01_kernel_1_1_movetk_point_01_6_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkPoint&lt; Kernel, const typename Kernel::MovetkPoint &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_point_3_01_kernel_00_01const_01typename_01_kernel_1_1_movetk_point_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkPoint&lt; Kernel, typename Kernel::MovetkPoint &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_point_3_01_kernel_00_01typename_01_kernel_1_1_movetk_point_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkSegment</name>
    <filename>structmovetk__core_1_1is___movetk_segment.html</filename>
    <templarg>Kernel</templarg>
    <templarg>Type</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkSegment&lt; Kernel, const typename Kernel::MovetkSegment &amp; &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_segment_3_01_kernel_00_01const_01typename_01_kernel_1_1_movetk_segment_01_6_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_MovetkSegment&lt; Kernel, typename Kernel::MovetkSegment &gt;</name>
    <filename>structmovetk__core_1_1is___movetk_segment_3_01_kernel_00_01typename_01_kernel_1_1_movetk_segment_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_NT</name>
    <filename>structmovetk__core_1_1is___n_t.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_NT&lt; Kernel, const typename Kernel::NT &amp; &gt;</name>
    <filename>structmovetk__core_1_1is___n_t_3_01_kernel_00_01const_01typename_01_kernel_1_1_n_t_01_6_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_NT&lt; Kernel, const typename Kernel::NT &gt;</name>
    <filename>structmovetk__core_1_1is___n_t_3_01_kernel_00_01const_01typename_01_kernel_1_1_n_t_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_NT&lt; Kernel, typename Kernel::NT &gt;</name>
    <filename>structmovetk__core_1_1is___n_t_3_01_kernel_00_01typename_01_kernel_1_1_n_t_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_output_iterator</name>
    <filename>structmovetk__core_1_1is__output__iterator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_output_iterator&lt; std::output_iterator_tag &gt;</name>
    <filename>structmovetk__core_1_1is__output__iterator_3_01std_1_1output__iterator__tag_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_pair</name>
    <filename>structmovetk__core_1_1is__pair.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_pair&lt; std::pair&lt; T1, T2 &gt; &gt;</name>
    <filename>structmovetk__core_1_1is__pair_3_01std_1_1pair_3_01_t1_00_01_t2_01_4_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_planar_geometry</name>
    <filename>structmovetk__core_1_1is__planar__geometry.html</filename>
    <templarg>dimensions</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_planar_geometry&lt; 2 &gt;</name>
    <filename>structmovetk__core_1_1is__planar__geometry_3_012_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_random_access_iterator</name>
    <filename>structmovetk__core_1_1is__random__access__iterator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_random_access_iterator&lt; std::random_access_iterator_tag &gt;</name>
    <filename>structmovetk__core_1_1is__random__access__iterator_3_01std_1_1random__access__iterator__tag_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_simple_graph</name>
    <filename>structmovetk__core_1_1is__simple__graph.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_simple_graph&lt; ValueType, boost::unordered_set&lt; typename ValueType::MovetkPoint &gt; &gt;</name>
    <filename>structmovetk__core_1_1is__simple__graph_3_01_value_type_00_01boost_1_1unordered__set_3_01typenam5dc58a0f6faba5fc293e2e6561798567.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_simple_graph&lt; ValueType, std::set&lt; typename ValueType::MovetkPoint &gt; &gt;</name>
    <filename>structmovetk__core_1_1is__simple__graph_3_01_value_type_00_01std_1_1set_3_01typename_01_value_type_1_1_movetk_point_01_4_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_size_t</name>
    <filename>structmovetk__core_1_1is__size__t.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_size_t&lt; const std::size_t &gt;</name>
    <filename>structmovetk__core_1_1is__size__t_3_01const_01std_1_1size__t_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_size_t&lt; std::size_t &gt;</name>
    <filename>structmovetk__core_1_1is__size__t_3_01std_1_1size__t_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_string</name>
    <filename>structmovetk__core_1_1is__string.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_string&lt; std::string &gt;</name>
    <filename>structmovetk__core_1_1is__string_3_01std_1_1string_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_tuple</name>
    <filename>structmovetk__core_1_1is__tuple.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_tuple&lt; const std::tuple&lt; Types... &gt; &gt;</name>
    <filename>structmovetk__core_1_1is__tuple_3_01const_01std_1_1tuple_3_01_types_8_8_8_01_4_01_4.html</filename>
    <templarg>Types</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_tuple&lt; std::tuple&lt; Types... &gt; &gt;</name>
    <filename>structmovetk__core_1_1is__tuple_3_01std_1_1tuple_3_01_types_8_8_8_01_4_01_4.html</filename>
    <templarg>Types</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::is_valid_NT</name>
    <filename>structmovetk__support_1_1is__valid___n_t.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::is_valid_NT&lt; CGAL::Gmpfr &gt;</name>
    <filename>structmovetk__support_1_1is__valid___n_t_3_01_c_g_a_l_1_1_gmpfr_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::is_valid_NT&lt; CGAL::Gmpq &gt;</name>
    <filename>structmovetk__support_1_1is__valid___n_t_3_01_c_g_a_l_1_1_gmpq_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::is_valid_NT&lt; CGAL::MP_Float &gt;</name>
    <filename>structmovetk__support_1_1is__valid___n_t_3_01_c_g_a_l_1_1_m_p___float_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::is_valid_NT&lt; CGAL::Mpzf &gt;</name>
    <filename>structmovetk__support_1_1is__valid___n_t_3_01_c_g_a_l_1_1_mpzf_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::is_valid_NT&lt; long double &gt;</name>
    <filename>structmovetk__support_1_1is__valid___n_t_3_01long_01double_01_4.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_Wrapper_Point</name>
    <filename>structmovetk__core_1_1is___wrapper___point.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_Wrapper_Point&lt; Kernel, const typename Kernel::Wrapper_Point &amp; &gt;</name>
    <filename>structmovetk__core_1_1is___wrapper___point_3_01_kernel_00_01const_01typename_01_kernel_1_1_wrapper___point_01_6_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::is_Wrapper_Point&lt; Kernel, typename Kernel::Wrapper_Point &gt;</name>
    <filename>structmovetk__core_1_1is___wrapper___point_3_01_kernel_00_01typename_01_kernel_1_1_wrapper___point_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>HighFrequencyTrajectorySplitter::iterator</name>
    <filename>class_high_frequency_trajectory_splitter_1_1iterator.html</filename>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>class_high_frequency_trajectory_splitter_1_1iterator.html</anchorfile>
      <anchor>a9820e08a0533405c9b32f6fbc6996985</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>class_high_frequency_trajectory_splitter_1_1iterator.html</anchorfile>
      <anchor>a09c812de4f7c94ef019bf2cdfafa61cd</anchor>
      <arglist>(HighFrequencyTrajectorySplitter &amp;parent)</arglist>
    </member>
    <member kind="function">
      <type>iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>class_high_frequency_trajectory_splitter_1_1iterator.html</anchorfile>
      <anchor>aa6d71e32210f8986a0c9bed303e06c46</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csv::iterator</name>
    <filename>classcsv_1_1iterator.html</filename>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>classcsv_1_1iterator.html</anchorfile>
      <anchor>afc3fd9555e3aeaf8ff93d4e22cf17d3b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>classcsv_1_1iterator.html</anchorfile>
      <anchor>a6677728bec1b9ff827c2286566040029</anchor>
      <arglist>(csv &amp;parent)</arglist>
    </member>
    <member kind="function">
      <type>iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classcsv_1_1iterator.html</anchorfile>
      <anchor>a195e1e9ef4e82061c1aa0978560006d8</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Splitter::iterator</name>
    <filename>class_splitter_1_1iterator.html</filename>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>class_splitter_1_1iterator.html</anchorfile>
      <anchor>a0703433e77fe77fe7179f73bcdd592e8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>class_splitter_1_1iterator.html</anchorfile>
      <anchor>ad56d28aeb235beb557786dc7c8fe9bce</anchor>
      <arglist>(Splitter &amp;parent)</arglist>
    </member>
    <member kind="function">
      <type>iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>class_splitter_1_1iterator.html</anchorfile>
      <anchor>a9715fd3373e391152be6fdcdae9029aa</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TrajectoryReader::iterator</name>
    <filename>class_trajectory_reader_1_1iterator.html</filename>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>class_trajectory_reader_1_1iterator.html</anchorfile>
      <anchor>a0bac24f86fa5223325226cc4cbf0c42a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>iterator</name>
      <anchorfile>class_trajectory_reader_1_1iterator.html</anchorfile>
      <anchor>a8ae2adf9ec6d1c6307f53de59b03fa8f</anchor>
      <arglist>(TrajectoryReader &amp;parent)</arglist>
    </member>
    <member kind="function">
      <type>iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>class_trajectory_reader_1_1iterator.html</anchorfile>
      <anchor>a51c3765fce6a6f8cba3798a6d4e69f6e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::IteratorAccessor</name>
    <filename>structmovetk__algorithms_1_1_iterator_accessor.html</filename>
    <templarg>Trajectory</templarg>
    <member kind="function">
      <type>Trajectory::template FieldType&lt; Idx &gt;</type>
      <name>getField</name>
      <anchorfile>structmovetk__algorithms_1_1_iterator_accessor.html</anchorfile>
      <anchor>ad8e96efb3a8753faaf12a718fbde3653</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Self_t</type>
      <name>operator+</name>
      <anchorfile>structmovetk__algorithms_1_1_iterator_accessor.html</anchorfile>
      <anchor>af12784714921ecd9042f0414087d888e</anchor>
      <arglist>(int offset)</arglist>
    </member>
    <member kind="function">
      <type>Self_t &amp;</type>
      <name>operator++</name>
      <anchorfile>structmovetk__algorithms_1_1_iterator_accessor.html</anchorfile>
      <anchor>a60d94bd4c80052c816bee3f0b15cbc4c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>structmovetk__algorithms_1_1_iterator_accessor.html</anchorfile>
      <anchor>a0499f59144c7f2cb3f807fab6c559f18</anchor>
      <arglist>(const Self_t &amp;other) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Self_t</type>
      <name>start</name>
      <anchorfile>structmovetk__algorithms_1_1_iterator_accessor.html</anchorfile>
      <anchor>a824fdc3adaf42162020aab11184969e2</anchor>
      <arglist>(Trajectory &amp;trajectory)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Self_t</type>
      <name>end</name>
      <anchorfile>structmovetk__algorithms_1_1_iterator_accessor.html</anchorfile>
      <anchor>a740ded50837c6e0ffd841a61482bdcf0</anchor>
      <arglist>(Trajectory &amp;trajectory)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>linear_speed_bounded_test_tag</name>
    <filename>structlinear__speed__bounded__test__tag.html</filename>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::detail::discrete_frechet_distance::linestring_linestring</name>
    <filename>structboost_1_1geometry_1_1detail_1_1discrete__frechet__distance_1_1linestring__linestring.html</filename>
  </compound>
  <compound kind="struct">
    <name>LocalCoordinateReference</name>
    <filename>struct_local_coordinate_reference.html</filename>
    <templarg>NT</templarg>
  </compound>
  <compound kind="class">
    <name>SEB_NAMESPACE::Logger</name>
    <filename>class_s_e_b___n_a_m_e_s_p_a_c_e_1_1_logger.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::brownian_bridge::LogLikelihood</name>
    <filename>classmovetk__algorithms_1_1brownian__bridge_1_1_log_likelihood.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::LongestCommonSubSequence</name>
    <filename>classmovetk__algorithms_1_1_longest_common_sub_sequence.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>TabularTrajectory::LookupByOffsetFn</name>
    <filename>class_tabular_trajectory_1_1_lookup_by_offset_fn.html</filename>
    <templarg></templarg>
    <templarg>LatIdx</templarg>
    <templarg>LonIdx</templarg>
    <member kind="function">
      <type>TrajectoryIterator</type>
      <name>operator()</name>
      <anchorfile>class_tabular_trajectory_1_1_lookup_by_offset_fn.html</anchorfile>
      <anchor>a644ad970c427be16052ea1f7f4e15983</anchor>
      <arglist>(double offset, double &amp;offset_begin, double &amp;offset_end)</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; double &gt;</type>
      <name>reverse_lookup</name>
      <anchorfile>class_tabular_trajectory_1_1_lookup_by_offset_fn.html</anchorfile>
      <anchor>ad0990490c6211879d165ebe3920f08ec</anchor>
      <arglist>(TrajectoryIterator it)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MakeLine</name>
    <filename>structmovetk__core_1_1_make_line.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>GeometryTraits::MovetkLine</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_make_line.html</anchorfile>
      <anchor>a7b857d34e15c3a152879dfeb6f7ed007</anchor>
      <arglist>(typename GeometryTraits::MovetkPoint p1, typename GeometryTraits::MovetkPoint p2)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MakeMinSphere</name>
    <filename>structmovetk__core_1_1_make_min_sphere.html</filename>
    <templarg>GeometryTraits</templarg>
    <member kind="function">
      <type>GeometryTraits::NT</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_make_min_sphere.html</anchorfile>
      <anchor>a3248aee5c6cd2f784fa5fac2579dd81c</anchor>
      <arglist>(PointIterator first, PointIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>GeometryTraits::NT</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_make_min_sphere.html</anchorfile>
      <anchor>acb7e64565ca57528776432055eeeb080</anchor>
      <arglist>(PointIterator first, PointIterator beyond, CenterIterator iter)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MakePoint</name>
    <filename>structmovetk__core_1_1_make_point.html</filename>
    <templarg>GeometryTraits</templarg>
    <member kind="function">
      <type>GeometryTraits::MovetkPoint</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_make_point.html</anchorfile>
      <anchor>aa62ecca7a0bc11239297577d3083ea3e</anchor>
      <arglist>(CoordinateIterator first, CoordinateIterator beyond)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MakePoint&lt; typename InterpolationTraits::GeometryTraits &gt;</name>
    <filename>structmovetk__core_1_1_make_point.html</filename>
    <member kind="function">
      <type>typename InterpolationTraits::GeometryTraits ::MovetkPoint</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_make_point.html</anchorfile>
      <anchor>aa62ecca7a0bc11239297577d3083ea3e</anchor>
      <arglist>(CoordinateIterator first, CoordinateIterator beyond)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MakePolygon</name>
    <filename>structmovetk__core_1_1_make_polygon.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>GeometryTraits::MovetkPolygon</type>
      <name>operator()</name>
      <anchorfile>structmovetk__core_1_1_make_polygon.html</anchorfile>
      <anchor>ac7eb346c500c5717c8bd58445fb5d1eb</anchor>
      <arglist>(PointIterator first, PointIterator beyond)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MakeSegment</name>
    <filename>structmovetk__core_1_1_make_segment.html</filename>
    <templarg>GeometryTraits</templarg>
  </compound>
  <compound kind="class">
    <name>MakeSegment&lt; typename _FreeSpaceDiagramTraits::GeometryTraits &gt;</name>
    <filename>structmovetk__core_1_1_make_segment.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MakeSphere</name>
    <filename>structmovetk__core_1_1_make_sphere.html</filename>
    <templarg>GeometryTraits</templarg>
  </compound>
  <compound kind="class">
    <name>MakeSphere&lt; typename CellTraits::GeometryTraits &gt;</name>
    <filename>structmovetk__core_1_1_make_sphere.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_core::MBR</name>
    <filename>classmovetk__core_1_1_m_b_r.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::mbr_selector</name>
    <filename>structmovetk__core_1_1mbr__selector.html</filename>
    <templarg>GeometryTraits</templarg>
    <templarg>Norm</templarg>
    <templarg>T</templarg>
  </compound>
  <compound kind="class">
    <name>mbr_selector&lt; GeometryTraits, Norm, typename GeometryTraits::MovetkMinimumBoundingRectangle &gt;</name>
    <filename>structmovetk__core_1_1mbr__selector.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::mbr_selector&lt; GeometryTraits, Norm, void &gt;</name>
    <filename>structmovetk__core_1_1mbr__selector_3_01_geometry_traits_00_01_norm_00_01void_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::brownian_bridge::MLE</name>
    <filename>classmovetk__algorithms_1_1brownian__bridge_1_1_m_l_e.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>MaxIter</templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>MLE</name>
      <anchorfile>classmovetk__algorithms_1_1brownian__bridge_1_1_m_l_e.html</anchorfile>
      <anchor>a5c992cb0e9c8ec77cd0c6fd412918d66</anchor>
      <arglist>(InputIterator first, InputIterator beyond, NT result, NT x_lower, NT x_upper, NT eps)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MLE</name>
      <anchorfile>classmovetk__algorithms_1_1brownian__bridge_1_1_m_l_e.html</anchorfile>
      <anchor>af45bcc35decbf8ad20e6e673f7c522ff</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>NT</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1brownian__bridge_1_1_m_l_e.html</anchorfile>
      <anchor>a85f9bd26413c63abe263c5295b499fbc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::brownian_bridge::Model</name>
    <filename>classmovetk__algorithms_1_1brownian__bridge_1_1_model.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>Model</name>
      <anchorfile>classmovetk__algorithms_1_1brownian__bridge_1_1_model.html</anchorfile>
      <anchor>ab27b7bf6c374c999992a1c559f15e3ee</anchor>
      <arglist>(TrajectoryIterator first, TrajectoryIterator beyond, OutputIterator result)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::ModelBasedSegmentation</name>
    <filename>classmovetk__algorithms_1_1_model_based_segmentation.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::MonotoneSegmentation</name>
    <filename>classmovetk__algorithms_1_1_monotone_segmentation.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>MonotoneSegmentation</name>
      <anchorfile>classmovetk__algorithms_1_1_monotone_segmentation.html</anchorfile>
      <anchor>ae838a032497c515df189d2d707485280</anchor>
      <arglist>(NT threshold)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_monotone_segmentation.html</anchorfile>
      <anchor>aca20fb67bdd38a7612834ee022a032fc</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator result)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_core::movetk_back_insert_iterator</name>
    <filename>classmovetk__core_1_1movetk__back__insert__iterator.html</filename>
    <templarg>Container</templarg>
    <member kind="function">
      <type></type>
      <name>movetk_back_insert_iterator</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>a01683a64eaa314b388b3c65bfda6e6d3</anchor>
      <arglist>(Container &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator &amp;</type>
      <name>operator=</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>ad1fb7a5cc849b9fd02a82c73bb428f08</anchor>
      <arglist>(const typename Container::value_type &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator &amp;</type>
      <name>operator*</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>a428dbd90fed24d44784b57eee7e3829f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>ae7d322f3d0dd59b9291f08e863af828e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator</type>
      <name>operator++</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>ad24c5a315d01bf94b45890383d1cbbdb</anchor>
      <arglist>(int)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_back_insert_iterator&lt; NodeType &gt;</name>
    <filename>classmovetk__core_1_1movetk__back__insert__iterator.html</filename>
    <member kind="function">
      <type></type>
      <name>movetk_back_insert_iterator</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>a01683a64eaa314b388b3c65bfda6e6d3</anchor>
      <arglist>(NodeType &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator &amp;</type>
      <name>operator=</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>ad1fb7a5cc849b9fd02a82c73bb428f08</anchor>
      <arglist>(const typename NodeType ::value_type &amp;value)</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator &amp;</type>
      <name>operator*</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>a428dbd90fed24d44784b57eee7e3829f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>ae7d322f3d0dd59b9291f08e863af828e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>movetk_back_insert_iterator</type>
      <name>operator++</name>
      <anchorfile>classmovetk__core_1_1movetk__back__insert__iterator.html</anchorfile>
      <anchor>ad24c5a315d01bf94b45890383d1cbbdb</anchor>
      <arglist>(int)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_core::movetk_basic_iterator</name>
    <filename>classmovetk__core_1_1movetk__basic__iterator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_core::movetk_grid_iterator</name>
    <filename>classmovetk__core_1_1movetk__grid__iterator.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_core::movetk_insert_iterator</name>
    <filename>classmovetk__core_1_1movetk__insert__iterator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::MovetkGeometryKernel</name>
    <filename>structmovetk__core_1_1_movetk_geometry_kernel.html</filename>
    <templarg></templarg>
    <member kind="typedef">
      <type>WrapperGeometryKernel::NT</type>
      <name>NT</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>ac60a50ecd8de035e89ba454d435b680b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Point</type>
      <name>MovetkPoint</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>aff274489e94eeea0f61ae1d0e5a43be8</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Line</type>
      <name>MovetkLine</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a9261d3e9262b4c7a95ade054538abc9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Segment</type>
      <name>MovetkSegment</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a7a315f61ca04c5497cc69ddc31eef86d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Vector</type>
      <name>MovetkVector</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a8c5a28e795a45508031e48752c2331a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_MinSphere</type>
      <name>MovetkMinSphere</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a1dedc207e4ac0c71eb393bbaec4b1b80</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Polygon</type>
      <name>MovetkPolygon</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a6f6a99dbe1f486c72d16cd41049b739a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Sphere</type>
      <name>MovetkSphere</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a63f1168d7739cb4538d68818448f1c67</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Intersection_visitor</type>
      <name>MovetkIntersectionVisitor</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a067e018419afdaa50d37343ed85d011b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Squared_Distance</type>
      <name>MovetkSquaredDistance</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>aa7ed44286026e56a8dc067ae8e730d7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Curve_Intersection</type>
      <name>MovetkCurveIntersection</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a79f66afa2d964c35170d72d0bc72cd8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Discrete_Hausdorff_Distance</type>
      <name>MovetkDiscreteHausdorffDistance</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a1579f1e7a73566cf27faab39bf37f0a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>WrapperGeometryKernel::Wrapper_Discrete_Frechet_Distance</type>
      <name>MovetkDiscreteFrechetDistance</name>
      <anchorfile>structmovetk__core_1_1_movetk_geometry_kernel.html</anchorfile>
      <anchor>a9f8f02296748ef21f01bc6d0bbc556dc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::detail::discrete_hausdorff_distance::multi_range_multi_range</name>
    <filename>structboost_1_1geometry_1_1detail_1_1discrete__hausdorff__distance_1_1multi__range__multi__range.html</filename>
  </compound>
  <compound kind="struct">
    <name>NotGroupedHighFrequencyTrajectoryReader</name>
    <filename>struct_not_grouped_high_frequency_trajectory_reader.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::OutlierDetection</name>
    <filename>classmovetk__algorithms_1_1_outlier_detection.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::OutlierDetection&lt; movetk_algorithms::greedy_outlier_detector_tag, Test, T &gt;</name>
    <filename>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1greedy__outlier__detector__tag_00_01_test_00_01_t_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>OutlierDetection</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1greedy__outlier__detector__tag_00_01_test_00_01_t_01_4.html</anchorfile>
      <anchor>a30984236dfe2e2d6e3376d04fb4ae1d9</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1greedy__outlier__detector__tag_00_01_test_00_01_t_01_4.html</anchorfile>
      <anchor>afda99c302b3a6497069ac316b762c0bf</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator result)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::OutlierDetection&lt; movetk_algorithms::output_sensitive_outlier_detector_tag, Test, T &gt;</name>
    <filename>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1output__sensitive__outli14f92ebbf044cdec7d6fa6fdb0259ece.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>OutlierDetection</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1output__sensitive__outli14f92ebbf044cdec7d6fa6fdb0259ece.html</anchorfile>
      <anchor>a868bd9b69ea47ccb8149e4263e7a5b45</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1output__sensitive__outli14f92ebbf044cdec7d6fa6fdb0259ece.html</anchorfile>
      <anchor>a05f6cc3715d8e188540c3c1a64b4b199</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator result)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::OutlierDetection&lt; movetk_algorithms::smart_greedy_outlier_detector_tag, Test, T &gt;</name>
    <filename>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1smart__greedy__outlier__b38dd0c7e4eae00ad0d433a374d7a747.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>OutlierDetection</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1smart__greedy__outlier__b38dd0c7e4eae00ad0d433a374d7a747.html</anchorfile>
      <anchor>a07c9551fdf57c95a99f6ca8e0237b7d7</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
    <member kind="function">
      <type>Container::const_iterator</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1smart__greedy__outlier__b38dd0c7e4eae00ad0d433a374d7a747.html</anchorfile>
      <anchor>a4cd0fc69fc9bd51f601e4290f2c3f9de</anchor>
      <arglist>(InputIterator first, InputIterator beyond, Container &amp;sequences)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::OutlierDetection&lt; movetk_algorithms::zheng_outlier_detector_tag, Test, T &gt;</name>
    <filename>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1zheng__outlier__detector__tag_00_01_test_00_01_t_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>OutlierDetection</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1zheng__outlier__detector__tag_00_01_test_00_01_t_01_4.html</anchorfile>
      <anchor>aa6fb65a7509dcda6e19b9256cb90ce5c</anchor>
      <arglist>(NT InThreshold, NT InMinSegSize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_outlier_detection_3_01movetk__algorithms_1_1zheng__outlier__detector__tag_00_01_test_00_01_t_01_4.html</anchorfile>
      <anchor>a09d554cce0f96e387f4236a982513ac7</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator result)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::OutlierDetectionTraits</name>
    <filename>structmovetk__algorithms_1_1_outlier_detection_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>output_sensitive_outlier_detector_tag</name>
    <filename>structoutput__sensitive__outlier__detector__tag.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep</name>
    <filename>structmovetk__support_1_1_output_rep.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Point &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01false_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___point_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Polygon &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01false_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___polygon_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Sphere &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01false_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___sphere_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Vector &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01false_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___vector_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Point &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01true_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___point_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Polygon &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01true_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___polygon_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Sphere &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01true_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___sphere_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Vector &gt;</name>
    <filename>structmovetk__support_1_1_output_rep_3_01true_00_01_kernel_00_01typename_01_kernel_1_1_wrapper___vector_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::brownian_bridge::ParameterSelector</name>
    <filename>classmovetk__algorithms_1_1brownian__bridge_1_1_parameter_selector.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>ParameterSelector</name>
      <anchorfile>classmovetk__algorithms_1_1brownian__bridge_1_1_parameter_selector.html</anchorfile>
      <anchor>afdadfb6a00b72fe56c7ea168a9445f0b</anchor>
      <arglist>(std::size_t size)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1brownian__bridge_1_1_parameter_selector.html</anchorfile>
      <anchor>a000126baee73f76e2a8b0580b300693a</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator result)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::brownian_bridge::ParameterTraits</name>
    <filename>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="enumeration">
      <type></type>
      <name>ParameterColumns</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>POINT</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba951c76b82f6d7f36fd030e186c3f0805</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MU</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba0b954df60e5361ab03a304c3c078ba9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SIGMA_SQUARED</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba8443381d1395461b2fd466dd4ee755ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BEGIN</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba04df40659674a8b0fee9df0f3cacfca3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>END</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50bafe5e547b53112d650e5bc6266c7afb0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>GeometryTraits::NT</type>
      <name>NT</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>a195ec3966f371e0dac9d14492079d8df</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>GeometryTraits::MovetkPoint</type>
      <name>Point</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>a3f1d87ec57a567e01398e823afb5786f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>GeometryTraits::MovetkVector</type>
      <name>Vector</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>a7ede1541a95f3d0ce91a89a1835498e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::tuple&lt; Point, Point, NT, TrajectoryIterator, TrajectoryIterator &gt;</type>
      <name>Parameters</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>a191c52e01bfa8bdfe34b3a33fdd16daa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>POINT</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba951c76b82f6d7f36fd030e186c3f0805</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MU</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba0b954df60e5361ab03a304c3c078ba9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SIGMA_SQUARED</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba8443381d1395461b2fd466dd4ee755ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BEGIN</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50ba04df40659674a8b0fee9df0f3cacfca3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>END</name>
      <anchorfile>structmovetk__algorithms_1_1brownian__bridge_1_1_parameter_traits.html</anchorfile>
      <anchor>adcf23b5b21df213ce8559df84b71c50bafe5e547b53112d650e5bc6266c7afb0b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ParseDate</name>
    <filename>class_parse_date.html</filename>
  </compound>
  <compound kind="class">
    <name>SEB_NAMESPACE::Point</name>
    <filename>class_s_e_b___n_a_m_e_s_p_a_c_e_1_1_point.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::detail::discrete_hausdorff_distance::point_range</name>
    <filename>structboost_1_1geometry_1_1detail_1_1discrete__hausdorff__distance_1_1point__range.html</filename>
  </compound>
  <compound kind="class">
    <name>geolife::c2d::raw::ProbeParseDate</name>
    <filename>classgeolife_1_1c2d_1_1raw_1_1_probe_parse_date.html</filename>
    <base>ParseDate</base>
  </compound>
  <compound kind="class">
    <name>movetk_kernel::ProbeParseDate</name>
    <filename>classmovetk__kernel_1_1_probe_parse_date.html</filename>
    <base>ParseDate</base>
  </compound>
  <compound kind="class">
    <name>here::c2d::raw::ProbeParseDate</name>
    <filename>classhere_1_1c2d_1_1raw_1_1_probe_parse_date.html</filename>
    <base>ParseDate</base>
  </compound>
  <compound kind="class">
    <name>ProbeReader</name>
    <filename>class_probe_reader.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>ProbeReaderFactory</name>
    <filename>class_probe_reader_factory.html</filename>
  </compound>
  <compound kind="class">
    <name>here::c2d::raw::ProviderCategoricalField</name>
    <filename>classhere_1_1c2d_1_1raw_1_1_provider_categorical_field.html</filename>
    <base>CategoricalField&lt; std::string, ProviderCategoricalField &gt;</base>
  </compound>
  <compound kind="class">
    <name>geolife::c2d::raw::ProviderCategoricalField</name>
    <filename>classgeolife_1_1c2d_1_1raw_1_1_provider_categorical_field.html</filename>
    <base>CategoricalField&lt; std::string, ProviderCategoricalField &gt;</base>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::detail::discrete_hausdorff_distance::range_multi_range</name>
    <filename>structboost_1_1geometry_1_1detail_1_1discrete__hausdorff__distance_1_1range__multi__range.html</filename>
  </compound>
  <compound kind="struct">
    <name>boost::geometry::detail::discrete_hausdorff_distance::range_range</name>
    <filename>structboost_1_1geometry_1_1detail_1_1discrete__hausdorff__distance_1_1range__range.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_core::Scaling</name>
    <filename>structmovetk__core_1_1_scaling.html</filename>
    <templarg>GeometryTraits</templarg>
  </compound>
  <compound kind="class">
    <name>Scaling&lt; typename InterpolationTraits::GeometryTraits &gt;</name>
    <filename>structmovetk__core_1_1_scaling.html</filename>
  </compound>
  <compound kind="struct">
    <name>movetk_algorithms::SegmentationTraits</name>
    <filename>structmovetk__algorithms_1_1_segmentation_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>dimensions</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_core::SegmentIdGenerator</name>
    <filename>classmovetk__core_1_1_segment_id_generator.html</filename>
    <templarg>PolyLineIdxIterator</templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>SegmentIdGenerator</name>
      <anchorfile>classmovetk__core_1_1_segment_id_generator.html</anchorfile>
      <anchor>a6a860c6358fdb1cc6ad7cacc84c1eb17</anchor>
      <arglist>(PolyLineIdxIterator first, PolyLineIdxIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getSegmentID</name>
      <anchorfile>classmovetk__core_1_1_segment_id_generator.html</anchorfile>
      <anchor>a436ab6c5b294a577a1284f2c5d5e5283</anchor>
      <arglist>(PolyLineIteratorType iter)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SEB_NAMESPACE::Smallest_enclosing_ball</name>
    <filename>class_s_e_b___n_a_m_e_s_p_a_c_e_1_1_smallest__enclosing__ball.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>smart_greedy_outlier_detector_tag</name>
    <filename>structsmart__greedy__outlier__detector__tag.html</filename>
  </compound>
  <compound kind="class">
    <name>SortByField</name>
    <filename>class_sort_by_field.html</filename>
    <templarg>FieldIndex</templarg>
    <templarg>ProbePoint</templarg>
    <templarg>asc</templarg>
  </compound>
  <compound kind="class">
    <name>SortedProbeReader</name>
    <filename>class_sorted_probe_reader.html</filename>
    <templarg></templarg>
    <templarg>SortByFieldIdx</templarg>
  </compound>
  <compound kind="class">
    <name>SplitByDifferenceThreshold</name>
    <filename>class_split_by_difference_threshold.html</filename>
    <templarg>FieldIndex</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>SplitByDistanceThreshold</name>
    <filename>class_split_by_distance_threshold.html</filename>
    <templarg>LatFieldIndex</templarg>
    <templarg>LonFieldIndex</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>SplitByField</name>
    <filename>class_split_by_field.html</filename>
    <templarg>FieldIndex</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>Splitter</name>
    <filename>class_splitter.html</filename>
    <templarg>Predicate</templarg>
    <templarg>ProbeInputIterator</templarg>
    <class kind="class">Splitter::iterator</class>
    <member kind="function">
      <type>bool</type>
      <name>good</name>
      <anchorfile>class_splitter.html</anchorfile>
      <anchor>ab06631b0532edc235bd63e5c8fa53664</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Splitter&lt; SplitByField, ProbeInputIterator &gt;</name>
    <filename>class_splitter.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>good</name>
      <anchorfile>class_splitter.html</anchorfile>
      <anchor>ab06631b0532edc235bd63e5c8fa53664</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::squared_distance_algorithm</name>
    <filename>structmovetk__core_1_1squared__distance__algorithm.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_core::squared_distance_algorithm&lt; GeometryTraits, Norm, void &gt;</name>
    <filename>structmovetk__core_1_1squared__distance__algorithm_3_01_geometry_traits_00_01_norm_00_01void_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::squared_distance_d</name>
    <filename>structmovetk__support_1_1squared__distance__d.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::StackTraits</name>
    <filename>structmovetk__support_1_1_stack_traits.html</filename>
    <templarg></templarg>
    <templarg>Container</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::StartStopDiagram</name>
    <filename>classmovetk__support_1_1_start_stop_diagram.html</filename>
    <templarg>SsdType</templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::StartStopDiagram&lt; SsdType::compressed, GeometryTraits, NodeType &gt;</name>
    <filename>classmovetk__support_1_1_start_stop_diagram_3_01_ssd_type_1_1compressed_00_01_geometry_traits_00_01_node_type_01_4.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_start_stop_diagram_3_01_ssd_type_1_1compressed_00_01_geometry_traits_00_01_node_type_01_4.html</anchorfile>
      <anchor>aaa62dad091a614051b481efdac6057e5</anchor>
      <arglist>(InputIterator first, InputIterator beyond, movetk_core::SegmentIdGenerator&lt; PolylineIdxIterator &gt; &amp;segments)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csvtools::std</name>
    <filename>classcsvtools_1_1std.html</filename>
    <templarg>idx</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>SEB_NAMESPACE::Subspan</name>
    <filename>class_s_e_b___n_a_m_e_s_p_a_c_e_1_1_subspan.html</filename>
    <templarg>Float</templarg>
    <templarg>Pt</templarg>
    <templarg>PointAccessor</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::SubTrajectoryClustering</name>
    <filename>classmovetk__algorithms_1_1_sub_trajectory_clustering.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>TabularTrajectory</name>
    <filename>class_tabular_trajectory.html</filename>
    <templarg>fields</templarg>
    <class kind="class">TabularTrajectory::FieldIterator</class>
    <class kind="class">TabularTrajectory::LookupByOffsetFn</class>
    <member kind="typedef">
      <type>typename std::vector&lt; std::tuple&lt; fields... &gt; &gt;::iterator</type>
      <name>TrajectoryIterator</name>
      <anchorfile>class_tabular_trajectory.html</anchorfile>
      <anchor>a76066731e998380ff06f5a4f615f8ec3</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; std::tuple&lt; fields... &gt; &gt; &amp;</type>
      <name>data</name>
      <anchorfile>class_tabular_trajectory.html</anchorfile>
      <anchor>a9a734384e082e7192d7863d9961bee50</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert</name>
      <anchorfile>class_tabular_trajectory.html</anchorfile>
      <anchor>a548145570bd5f31535fcef63aaa95a03</anchor>
      <arglist>(ConstTrajectoryIterator pos, TrajectoryIterator first, TrajectoryIterator last)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; value_type, TrajectoryIterator &gt;</type>
      <name>getInterpolatedPoint</name>
      <anchorfile>class_tabular_trajectory.html</anchorfile>
      <anchor>ae69971ff5425d59eb4d018768aebe5b1</anchor>
      <arglist>(double offset, Interpolator_2 interpolator)</arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>class_tabular_trajectory.html</anchorfile>
      <anchor>ac10e5947fe58e5253b9afbabcbade8fb</anchor>
      <arglist>(std::ostream &amp;os, TabularTrajectory&lt; fields... &gt; &amp;trajectory)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>template&lt; class GeometryTraits, class ParameterTraits, class Norm, class InputIterator, std::size_t MaxIter &gt;</name>
    <filename>classtemplate_3_01class_01_geometry_traits_00_01class_01_parameter_traits_00_01class_01_norm_00_54301d4ca90d30d3a40053789e00b9bd.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::outlier_detection::TEST</name>
    <filename>classmovetk__algorithms_1_1outlier__detection_1_1_t_e_s_t.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TEST</name>
    <filename>classmovetk__algorithms_1_1_t_e_s_t.html</filename>
    <templarg>TestCriteria</templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::outlier_detection::TEST&lt; movetk_algorithms::linear_speed_bounded_test_tag, movetk_algorithms::cartesian_coordinates_tag, T &gt;</name>
    <filename>classmovetk__algorithms_1_1outlier__detection_1_1_t_e_s_t_3_01movetk__algorithms_1_1linear__spee380857ef8cce108ad0a911fa7db1f3e6.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>classmovetk__algorithms_1_1outlier__detection_1_1_t_e_s_t_3_01movetk__algorithms_1_1linear__spee380857ef8cce108ad0a911fa7db1f3e6.html</anchorfile>
      <anchor>a8b9a10b249a1b9fb41bfc5f506864265</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1outlier__detection_1_1_t_e_s_t_3_01movetk__algorithms_1_1linear__spee380857ef8cce108ad0a911fa7db1f3e6.html</anchorfile>
      <anchor>a5e8ec2207bd5b978ffbb490f367e7079</anchor>
      <arglist>(const T1 &amp;p1, const T1 &amp;p2)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::outlier_detection::TEST&lt; movetk_algorithms::linear_speed_bounded_test_tag, movetk_algorithms::geo_coordinates_tag, T &gt;</name>
    <filename>classmovetk__algorithms_1_1outlier__detection_1_1_t_e_s_t_3_01movetk__algorithms_1_1linear__spee7a48e97f03851664f7c2038dfc77677e.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>classmovetk__algorithms_1_1outlier__detection_1_1_t_e_s_t_3_01movetk__algorithms_1_1linear__spee7a48e97f03851664f7c2038dfc77677e.html</anchorfile>
      <anchor>a4b0a6b8a041ee9f8264c6976fdff062f</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TEST&lt; TestCriteria::range, GeometryTraits &gt;</name>
    <filename>classmovetk__algorithms_1_1_t_e_s_t_3_01_test_criteria_1_1range_00_01_geometry_traits_01_4.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_01_test_criteria_1_1range_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>a290cd0bb588618b44fd8acd5eacff960</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_01_test_criteria_1_1range_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>a31ce20c109493782d1ff60f14f64e4cb</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TEST&lt;::TestCriteria::difference, GeometryTraits &gt;</name>
    <filename>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1difference_00_01_geometry_traits_01_4.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1difference_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>adccc0fda2eddd612d0eb674b1aa39688</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1difference_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>a5dbc3055f149b26fabe29ace1e626c4c</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TEST&lt;::TestCriteria::meb, GeometryTraits &gt;</name>
    <filename>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1meb_00_01_geometry_traits_01_4.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1meb_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>a62fd288f419dc8909f5517f17fabf664</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1meb_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>a0f375866210031740c3974cfed2c07e0</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TEST&lt;::TestCriteria::ratio, GeometryTraits &gt;</name>
    <filename>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1ratio_00_01_geometry_traits_01_4.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1ratio_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>a5c4a2ab57b4ffef9b3226c58c062b3b1</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>classmovetk__algorithms_1_1_t_e_s_t_3_1_1_test_criteria_1_1ratio_00_01_geometry_traits_01_4.html</anchorfile>
      <anchor>aa6faf7a30de80e6350398b7d4beb2f21</anchor>
      <arglist>(NT InThreshold)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SEB_NAMESPACE::Timer</name>
    <filename>class_s_e_b___n_a_m_e_s_p_a_c_e_1_1_timer.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TrajectoryDuration</name>
    <filename>classmovetk__algorithms_1_1_trajectory_duration.html</filename>
    <templarg></templarg>
    <templarg>TimeStampIdx</templarg>
    <member kind="function">
      <type>Duration_t</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_trajectory_duration.html</anchorfile>
      <anchor>aa9834ae8f225a5b9991704a8bf2f8aa5</anchor>
      <arglist>(Trajectory trajectory) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TrajectoryLength</name>
    <filename>classmovetk__algorithms_1_1_trajectory_length.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>XcoordIdx</templarg>
    <templarg>YcoordIdx</templarg>
  </compound>
  <compound kind="class">
    <name>TrajectoryReader</name>
    <filename>class_trajectory_reader.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>SortTrajectory</templarg>
    <templarg>RemoveDuplicates</templarg>
    <class kind="class">TrajectoryReader::iterator</class>
    <member kind="function">
      <type>bool</type>
      <name>good</name>
      <anchorfile>class_trajectory_reader.html</anchorfile>
      <anchor>a084c7f46c41b5e0b7c884b59e02460ae</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TrajectorySpeedStatistic</name>
    <filename>classmovetk__algorithms_1_1_trajectory_speed_statistic.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>XcoordIdx</templarg>
    <templarg>YcoordIdx</templarg>
    <templarg>TimeStampIdx</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_algorithms::TrajectoryTimeIntervalMode</name>
    <filename>classmovetk__algorithms_1_1_trajectory_time_interval_mode.html</filename>
    <templarg></templarg>
    <templarg>TimeStampIdx</templarg>
    <member kind="function">
      <type></type>
      <name>TrajectoryTimeIntervalMode</name>
      <anchorfile>classmovetk__algorithms_1_1_trajectory_time_interval_mode.html</anchorfile>
      <anchor>a3f626c5ccac1be757bbe19b188655fea</anchor>
      <arglist>(int binNum=5)</arglist>
    </member>
    <member kind="function">
      <type>Duration_t</type>
      <name>operator()</name>
      <anchorfile>classmovetk__algorithms_1_1_trajectory_time_interval_mode.html</anchorfile>
      <anchor>aab2674ac743fb0ddcf0ee9d83daaa283</anchor>
      <arglist>(Trajectory trajectory)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_core::Translation</name>
    <filename>structmovetk__core_1_1_translation.html</filename>
    <templarg>GeometryTraits</templarg>
  </compound>
  <compound kind="class">
    <name>Translation&lt; typename InterpolationTraits::GeometryTraits &gt;</name>
    <filename>structmovetk__core_1_1_translation.html</filename>
  </compound>
  <compound kind="struct">
    <name>Transpose</name>
    <filename>struct_transpose.html</filename>
    <templarg>fields</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Tree</name>
    <filename>classmovetk__support_1_1_tree.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>Tree</name>
      <anchorfile>classmovetk__support_1_1_tree.html</anchorfile>
      <anchor>aea7fc501f4813b9b84df39a6bbb6915e</anchor>
      <arglist>(std::unique_ptr&lt; NodeType &gt; nd)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert</name>
      <anchorfile>classmovetk__support_1_1_tree.html</anchorfile>
      <anchor>a0c2f8d6eacc82d593fe5b6ed7f3b78aa</anchor>
      <arglist>(InputIterator first, InputIterator beyond, ValueType &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>reference</type>
      <name>find</name>
      <anchorfile>classmovetk__support_1_1_tree.html</anchorfile>
      <anchor>a9e1307c590f9379543a25d2096f38652</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>find</name>
      <anchorfile>classmovetk__support_1_1_tree.html</anchorfile>
      <anchor>a3b2bfcf5ec5096ed8e131af2e35e05b1</anchor>
      <arglist>(OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_match_size</name>
      <anchorfile>classmovetk__support_1_1_tree.html</anchorfile>
      <anchor>abc2c3431959536002579d917f3a86dcf</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::TrieNode</name>
    <filename>classmovetk__support_1_1_trie_node.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>TrieNode</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a5041508dbc2ae54b75d6baf2b401715b</anchor>
      <arglist>(K &amp;InKey)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a40604467b6186a2c5ce635167929f162</anchor>
      <arglist>(NodePtr nd)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a41b3d97a3525dff7331ebfccf1444d0d</anchor>
      <arglist>(K &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a755e61a13735eaad2d9a63be4255559a</anchor>
      <arglist>(reference nd)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a79a29cd07055e0be34b0dba4ca1b7b59</anchor>
      <arglist>(reference nd)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a543a9214b6103636636071b28ccdb2d6</anchor>
      <arglist>(reference nd)</arglist>
    </member>
    <member kind="function">
      <type>K</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a0e1d83a87c4b65ea1c4188f8c29b047c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>NodePtr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a3faef8a7bc5b4e2356f3f3bd39038e3b</anchor>
      <arglist>(K &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>NodePtr &amp;</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>ac065b4bde0939cc269711e501eec80cc</anchor>
      <arglist>(iterator iter)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>begin</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a8dcfeda7c58b74f938e0d1f2138f0635</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>end</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>ad2de92c2cdf35426b02e4007e5d3409d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert_value</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a8721b7c2716053f3c1628516c4a2778b</anchor>
      <arglist>(std::string &amp;id, T &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>ValuesIterator</type>
      <name>v_begin</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>a7c557574e231f68b8bd897ec5790bbce</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ValuesIterator</type>
      <name>v_end</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>aa958a2d6848fd8266dd52326dd652b6f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string &amp;</type>
      <name>v_id</name>
      <anchorfile>classmovetk__support_1_1_trie_node.html</anchorfile>
      <anchor>aeb92c4c1b19a2257818cc782186ccdb1</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_support::TrieTraits</name>
    <filename>structmovetk__support_1_1_trie_traits.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg>DataContainer</templarg>
    <templarg>LinkContainer</templarg>
  </compound>
  <compound kind="struct">
    <name>TupleIterPrinter</name>
    <filename>struct_tuple_iter_printer.html</filename>
    <templarg></templarg>
    <templarg>N</templarg>
  </compound>
  <compound kind="struct">
    <name>TupleIterPrinter&lt; Tuple, 1 &gt;</name>
    <filename>struct_tuple_iter_printer_3_01_tuple_00_011_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>TuplePrinter</name>
    <filename>struct_tuple_printer.html</filename>
    <templarg></templarg>
    <templarg>N</templarg>
  </compound>
  <compound kind="struct">
    <name>TuplePrinter&lt; Tuple, 1 &gt;</name>
    <filename>struct_tuple_printer_3_01_tuple_00_011_01_4.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_core::Wedge</name>
    <filename>classmovetk__core_1_1_wedge.html</filename>
    <templarg>GeometryTraits</templarg>
    <templarg>Norm</templarg>
    <templarg></templarg>
    <templarg></templarg>
    <member kind="function">
      <type>Wedge</type>
      <name>operator*</name>
      <anchorfile>classmovetk__core_1_1_wedge.html</anchorfile>
      <anchor>a650cdb87dbcb3a9865f76116da71d0fe</anchor>
      <arglist>(Wedge &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator*</name>
      <anchorfile>classmovetk__core_1_1_wedge.html</anchorfile>
      <anchor>a07bdb9c5cb88482c1977dcaa3babf692</anchor>
      <arglist>(typename GeometryTraits::MovetkPoint &amp;p)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>movetk_support::Wrapper_Boost_Discrete_Frechet_Distance</name>
    <filename>structmovetk__support_1_1_wrapper___boost___discrete___frechet___distance.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::Wrapper_Boost_Discrete_Hausdorff_Distance</name>
    <filename>structmovetk__support_1_1_wrapper___boost___discrete___hausdorff___distance.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::Wrapper_Boost_Kernel</name>
    <filename>structmovetk__support_1_1_wrapper___boost___kernel.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_Boost_Line</name>
    <filename>classmovetk__support_1_1_wrapper___boost___line.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_Boost_Point</name>
    <filename>classmovetk__support_1_1_wrapper___boost___point.html</filename>
    <templarg>Kernel</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_Boost_Polygon</name>
    <filename>classmovetk__support_1_1_wrapper___boost___polygon.html</filename>
    <templarg>Kernel</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_Boost_Segment</name>
    <filename>classmovetk__support_1_1_wrapper___boost___segment.html</filename>
    <templarg>Kernel</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_Boost_Sphere</name>
    <filename>classmovetk__support_1_1_wrapper___boost___sphere.html</filename>
    <templarg>Kernel</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::Wrapper_Boost_Squared_Distance</name>
    <filename>structmovetk__support_1_1_wrapper___boost___squared___distance.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_Boost_Vector</name>
    <filename>classmovetk__support_1_1_wrapper___boost___vector.html</filename>
    <templarg>Kernel</templarg>
  </compound>
  <compound kind="struct">
    <name>movetk_support::Wrapper_CGAL_Kernel</name>
    <filename>structmovetk__support_1_1_wrapper___c_g_a_l___kernel.html</filename>
    <templarg></templarg>
    <class kind="struct">movetk_support::Wrapper_CGAL_Kernel::Wrapper_CGAL_Kernel_2</class>
  </compound>
  <compound kind="struct">
    <name>movetk_support::Wrapper_CGAL_Kernel::Wrapper_CGAL_Kernel_2</name>
    <filename>structmovetk__support_1_1_wrapper___c_g_a_l___kernel_1_1_wrapper___c_g_a_l___kernel__2.html</filename>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_CGAL_Line</name>
    <filename>classmovetk__support_1_1_wrapper___c_g_a_l___line.html</filename>
    <templarg>Kernel</templarg>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Line</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___line.html</anchorfile>
      <anchor>a46e5b8caf7c1034afe635d8519752f5b</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Line</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___line.html</anchorfile>
      <anchor>ac6314662b6b1be26107d74f1fc35503e</anchor>
      <arglist>(const CGAL_Line &amp;l)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Line</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___line.html</anchorfile>
      <anchor>a6a7cb73ac3fb732f04ec27ddf4e9fe78</anchor>
      <arglist>(Wrapper_CGAL_Point&lt; Kernel &gt; &amp;p1, Wrapper_CGAL_Point&lt; Kernel &gt; &amp;p2)</arglist>
    </member>
    <member kind="function">
      <type>CGAL_Line</type>
      <name>get</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___line.html</anchorfile>
      <anchor>a4c8e4d08678288ee21b89781dc4ebd02</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_CGAL_Point</name>
    <filename>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</filename>
    <templarg>Kernel</templarg>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Point</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>a0830eddc07b49e415877d580c20ad676</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Point</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>a2b0d79a9d6799197872528775c3a6726</anchor>
      <arglist>(const CGAL_Point &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Point</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>ab302cb90b9fb0932c937b060e48af1d9</anchor>
      <arglist>(CoordinateIterator first, CoordinateIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>begin</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>a31f947d70bf85a57de35948821b96362</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>end</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>a427bc0a9ca062afd7eed26a450f49f28</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>a42acb90ecf38d4528cbe9794c82814f3</anchor>
      <arglist>(const Wrapper_CGAL_Point&lt; Kernel &gt; &amp;point)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>acf4a2c31daa8a503843757f7d66756f3</anchor>
      <arglist>(const Wrapper_CGAL_Point&lt; Kernel &gt; &amp;point)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>aac66dfa33d498d43fa1a981177b5900c</anchor>
      <arglist>(const Wrapper_CGAL_Point&lt; Kernel &gt; &amp;point) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>ae8255ecde4bb6b084489d1bfa50e0a3e</anchor>
      <arglist>(const Wrapper_CGAL_Point&lt; Kernel &gt; &amp;point)</arglist>
    </member>
    <member kind="function">
      <type>CGAL_Point</type>
      <name>get</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___point.html</anchorfile>
      <anchor>aa27f5d7bf7b1cd8b5126d903f52ee0a2</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_CGAL_Polygon</name>
    <filename>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</filename>
    <templarg>Kernel</templarg>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Polygon</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a100a2dd4c6ab0dfb2a938760af588523</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Polygon</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a4c6bf7b6c736f2c43f97fd37fbdd8cdd</anchor>
      <arglist>(PointIterator first, PointIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>CGAL_Polygon &amp;</type>
      <name>get</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a7c74701835cce6dd4b751f46e5ab5b5a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>v_begin</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a8626c28f1a64281c8d239e3b378925a3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>v_end</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a984e187c4e270773a161740895b5e516</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>e_begin</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a996830f3eeba67cc532cd5007c46f68a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>e_end</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___polygon.html</anchorfile>
      <anchor>a82ef8e646c611a8e104e439bbe85f555</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_CGAL_Segment</name>
    <filename>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Segment</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</anchorfile>
      <anchor>af476c6d67ee3cd88124d72bb5b5a1aad</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Segment</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</anchorfile>
      <anchor>ace22c1ac87985dd2fbde5fe7196af9b3</anchor>
      <arglist>(const CGAL_Segment &amp;s)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Segment</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</anchorfile>
      <anchor>a1cbb55ad6fdecce47aec1c9dd727e677</anchor>
      <arglist>(Wrapper_CGAL_Point&lt; Kernel &gt; &amp;p1, Wrapper_CGAL_Point&lt; Kernel &gt; &amp;p2)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Segment</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</anchorfile>
      <anchor>afd209d53da44e7f9a7c89bf3c8f03ceb</anchor>
      <arglist>(Wrapper_CGAL_Point&lt; Kernel &gt; &amp;&amp;p1, Wrapper_CGAL_Point&lt; Kernel &gt; &amp;&amp;p2)</arglist>
    </member>
    <member kind="function">
      <type>NT</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</anchorfile>
      <anchor>a1907bd52e128b91ceb7068eb2084160f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>CGAL_Segment</type>
      <name>get</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___segment.html</anchorfile>
      <anchor>a1e19941da2c48eb8236cd500ab5f788d</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_CGAL_Sphere</name>
    <filename>classmovetk__support_1_1_wrapper___c_g_a_l___sphere.html</filename>
    <templarg>Kernel</templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_CGAL_Vector</name>
    <filename>classmovetk__support_1_1_wrapper___c_g_a_l___vector.html</filename>
    <templarg>Kernel</templarg>
    <member kind="function">
      <type></type>
      <name>Wrapper_CGAL_Vector</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___vector.html</anchorfile>
      <anchor>a824d7d0f18dbea08dfee50488dc1d50c</anchor>
      <arglist>(Wrapper_CGAL_Point&lt; Kernel &gt; &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>begin</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___vector.html</anchorfile>
      <anchor>a9593c1ff1bf5e627628d9655b24ebcc6</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>end</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___vector.html</anchorfile>
      <anchor>aa3521d3c7534ba5a71dff60f759c51f3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>CGAL_Vector</type>
      <name>get</name>
      <anchorfile>classmovetk__support_1_1_wrapper___c_g_a_l___vector.html</anchorfile>
      <anchor>a88053af1222a342c65006d80edfe967f</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::CGAL_Algorithms::Wrapper_Curve_Intersection</name>
    <filename>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_wrapper___curve___intersection.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>movetk_support::CGAL_Algorithms::Wrapper_Min_Sphere</name>
    <filename>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_wrapper___min___sphere.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>NT</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_wrapper___min___sphere.html</anchorfile>
      <anchor>af533ee023181b0a89903c85905db5c57</anchor>
      <arglist>(PointIterator first, PointIterator beyond, CenterIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>NT</type>
      <name>operator()</name>
      <anchorfile>classmovetk__support_1_1_c_g_a_l___algorithms_1_1_wrapper___min___sphere.html</anchorfile>
      <anchor>a31e7d731885f4bae11f9912644d7fc2f</anchor>
      <arglist>(PointIterator first, PointIterator beyond)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>movetk_support::Wrapper_SEB</name>
    <filename>classmovetk__support_1_1_wrapper___s_e_b.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="struct">
    <name>zheng_outlier_detector_tag</name>
    <filename>structzheng__outlier__detector__tag.html</filename>
  </compound>
  <compound kind="class">
    <name>ZippedProbeReader</name>
    <filename>class_zipped_probe_reader.html</filename>
    <templarg></templarg>
    <base>ProbeReader</base>
  </compound>
  <compound kind="namespace">
    <name>movetk</name>
    <filename>namespacemovetk.html</filename>
    <member kind="function">
      <type>OutputIterator</type>
      <name>to_geocentered_polyline</name>
      <anchorfile>namespacemovetk.html</anchorfile>
      <anchor>ad6e4722dc7ac801aec91d86925975763</anchor>
      <arglist>(movetk_core::MakePoint&lt; GeometryTraits &gt; &amp;make_point, LatIterator lat_start, LatIterator lat_beyond, LonIterator lon_start, OutputIterator polyline_first)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>movetk_algorithms</name>
    <filename>namespacemovetk__algorithms.html</filename>
    <namespace>movetk_algorithms::brownian_bridge</namespace>
    <class kind="struct">movetk_algorithms::BBMMParameterTraits</class>
    <class kind="class">movetk_algorithms::BinarySearch</class>
    <class kind="class">movetk_algorithms::ChanChin</class>
    <class kind="struct">movetk_algorithms::ClusteringTraits</class>
    <class kind="class">movetk_algorithms::DouglasPeucker</class>
    <class kind="class">movetk_algorithms::DummyOffsetInterpolator_2</class>
    <class kind="class">movetk_algorithms::DynamicTimeWarping</class>
    <class kind="struct">movetk_algorithms::FindFarthest</class>
    <class kind="class">movetk_algorithms::ImaiIri</class>
    <class kind="struct">movetk_algorithms::InterpolationTraits</class>
    <class kind="class">movetk_algorithms::Interpolator</class>
    <class kind="class">movetk_algorithms::Interpolator&lt; movetk_algorithms::kinematic_interpolator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx &gt;</class>
    <class kind="class">movetk_algorithms::Interpolator&lt; movetk_algorithms::linear_interpolator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx &gt;</class>
    <class kind="class">movetk_algorithms::Interpolator&lt; movetk_algorithms::random_trajectory_generator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx &gt;</class>
    <class kind="struct">movetk_algorithms::is_monotone</class>
    <class kind="struct">movetk_algorithms::IteratorAccessor</class>
    <class kind="class">movetk_algorithms::LongestCommonSubSequence</class>
    <class kind="class">movetk_algorithms::ModelBasedSegmentation</class>
    <class kind="class">movetk_algorithms::MonotoneSegmentation</class>
    <class kind="class">movetk_algorithms::OutlierDetection</class>
    <class kind="class">movetk_algorithms::OutlierDetection&lt; movetk_algorithms::greedy_outlier_detector_tag, Test, T &gt;</class>
    <class kind="class">movetk_algorithms::OutlierDetection&lt; movetk_algorithms::output_sensitive_outlier_detector_tag, Test, T &gt;</class>
    <class kind="class">movetk_algorithms::OutlierDetection&lt; movetk_algorithms::smart_greedy_outlier_detector_tag, Test, T &gt;</class>
    <class kind="class">movetk_algorithms::OutlierDetection&lt; movetk_algorithms::zheng_outlier_detector_tag, Test, T &gt;</class>
    <class kind="struct">movetk_algorithms::OutlierDetectionTraits</class>
    <class kind="struct">movetk_algorithms::SegmentationTraits</class>
    <class kind="class">movetk_algorithms::SubTrajectoryClustering</class>
    <class kind="class">movetk_algorithms::TEST</class>
    <class kind="class">movetk_algorithms::TEST&lt; TestCriteria::range, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TEST&lt;::TestCriteria::difference, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TEST&lt;::TestCriteria::meb, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TEST&lt;::TestCriteria::ratio, GeometryTraits &gt;</class>
    <class kind="class">movetk_algorithms::TrajectoryDuration</class>
    <class kind="class">movetk_algorithms::TrajectoryLength</class>
    <class kind="class">movetk_algorithms::TrajectorySpeedStatistic</class>
    <class kind="class">movetk_algorithms::TrajectoryTimeIntervalMode</class>
    <member kind="function">
      <type>double</type>
      <name>polyline_length_m</name>
      <anchorfile>namespacemovetk__algorithms.html</anchorfile>
      <anchor>a694df30bccc15ef0eb2d097a7b1f2830</anchor>
      <arglist>(PointIterator start, PointIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; double &gt;</type>
      <name>polyline_lengths_m</name>
      <anchorfile>namespacemovetk__algorithms.html</anchorfile>
      <anchor>a2625116d7b182da0153bcd0742dabfad</anchor>
      <arglist>(PointIterator start, PointIterator beyond)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>movetk_algorithms::brownian_bridge</name>
    <filename>namespacemovetk__algorithms_1_1brownian__bridge.html</filename>
    <class kind="class">movetk_algorithms::brownian_bridge::LogLikelihood</class>
    <class kind="class">movetk_algorithms::brownian_bridge::MLE</class>
    <class kind="class">movetk_algorithms::brownian_bridge::Model</class>
    <class kind="class">movetk_algorithms::brownian_bridge::ParameterSelector</class>
    <class kind="struct">movetk_algorithms::brownian_bridge::ParameterTraits</class>
  </compound>
  <compound kind="namespace">
    <name>movetk_core</name>
    <filename>namespacemovetk__core.html</filename>
    <class kind="struct">movetk_core::ComputeDiscreteFrechetDistance</class>
    <class kind="struct">movetk_core::ComputeDiscreteHausdorffDistance</class>
    <class kind="class">movetk_core::ComputeIntersections</class>
    <class kind="struct">movetk_core::ComputeLength</class>
    <class kind="struct">movetk_core::ComputeSquaredDistance</class>
    <class kind="struct">movetk_core::discrete_frechet_distance_algorithm</class>
    <class kind="struct">movetk_core::discrete_frechet_distance_algorithm&lt; GeometryTraits, Norm, void &gt;</class>
    <class kind="struct">movetk_core::discrete_hausdorff_distance_algorithm</class>
    <class kind="struct">movetk_core::discrete_hausdorff_distance_algorithm&lt; GeometryTraits, Norm, void &gt;</class>
    <class kind="struct">movetk_core::IntersectionTraits</class>
    <class kind="struct">movetk_core::IntersectionTraits&lt; _GeometryTraits, _Norm, polylines_instersection_tag &gt;</class>
    <class kind="struct">movetk_core::IntersectionTraits&lt; _GeometryTraits, _Norm, sphere_segment_intersection_tag &gt;</class>
    <class kind="struct">movetk_core::IntersectionTraits&lt; _GeometryTraits, _Norm, sphere_sphere_intersection_tag &gt;</class>
    <class kind="struct">movetk_core::is_date</class>
    <class kind="struct">movetk_core::is_date&lt; const ParseDate &gt;</class>
    <class kind="struct">movetk_core::is_date&lt; ParseDate &gt;</class>
    <class kind="struct">movetk_core::is_date&lt; std::size_t &gt;</class>
    <class kind="struct">movetk_core::is_date&lt; std::time_t &gt;</class>
    <class kind="struct">movetk_core::is_directed_graph</class>
    <class kind="struct">movetk_core::is_directed_graph&lt; boost::directedS &gt;</class>
    <class kind="struct">movetk_core::is_equal_to</class>
    <class kind="struct">movetk_core::is_equal_to&lt; T1, T1 &gt;</class>
    <class kind="struct">movetk_core::is_forward_iterator</class>
    <class kind="struct">movetk_core::is_forward_iterator&lt; std::forward_iterator_tag &gt;</class>
    <class kind="struct">movetk_core::is_L2_norm</class>
    <class kind="struct">movetk_core::is_L2_norm&lt; 2 &gt;</class>
    <class kind="struct">movetk_core::is_MovetkLine</class>
    <class kind="struct">movetk_core::is_MovetkLine&lt; Kernel, const typename Kernel::MovetkLine &amp; &gt;</class>
    <class kind="struct">movetk_core::is_MovetkLine&lt; Kernel, typename Kernel::MovetkLine &gt;</class>
    <class kind="struct">movetk_core::is_MovetkPoint</class>
    <class kind="struct">movetk_core::is_MovetkPoint&lt; Kernel, const typename Kernel::MovetkPoint &amp; &gt;</class>
    <class kind="struct">movetk_core::is_MovetkPoint&lt; Kernel, const typename Kernel::MovetkPoint &gt;</class>
    <class kind="struct">movetk_core::is_MovetkPoint&lt; Kernel, typename Kernel::MovetkPoint &gt;</class>
    <class kind="struct">movetk_core::is_MovetkSegment</class>
    <class kind="struct">movetk_core::is_MovetkSegment&lt; Kernel, const typename Kernel::MovetkSegment &amp; &gt;</class>
    <class kind="struct">movetk_core::is_MovetkSegment&lt; Kernel, typename Kernel::MovetkSegment &gt;</class>
    <class kind="struct">movetk_core::is_NT</class>
    <class kind="struct">movetk_core::is_NT&lt; Kernel, const typename Kernel::NT &amp; &gt;</class>
    <class kind="struct">movetk_core::is_NT&lt; Kernel, const typename Kernel::NT &gt;</class>
    <class kind="struct">movetk_core::is_NT&lt; Kernel, typename Kernel::NT &gt;</class>
    <class kind="struct">movetk_core::is_output_iterator</class>
    <class kind="struct">movetk_core::is_output_iterator&lt; std::output_iterator_tag &gt;</class>
    <class kind="struct">movetk_core::is_pair</class>
    <class kind="struct">movetk_core::is_pair&lt; std::pair&lt; T1, T2 &gt; &gt;</class>
    <class kind="struct">movetk_core::is_planar_geometry</class>
    <class kind="struct">movetk_core::is_planar_geometry&lt; 2 &gt;</class>
    <class kind="struct">movetk_core::is_random_access_iterator</class>
    <class kind="struct">movetk_core::is_random_access_iterator&lt; std::random_access_iterator_tag &gt;</class>
    <class kind="struct">movetk_core::is_simple_graph</class>
    <class kind="struct">movetk_core::is_simple_graph&lt; ValueType, boost::unordered_set&lt; typename ValueType::MovetkPoint &gt; &gt;</class>
    <class kind="struct">movetk_core::is_simple_graph&lt; ValueType, std::set&lt; typename ValueType::MovetkPoint &gt; &gt;</class>
    <class kind="struct">movetk_core::is_size_t</class>
    <class kind="struct">movetk_core::is_size_t&lt; const std::size_t &gt;</class>
    <class kind="struct">movetk_core::is_size_t&lt; std::size_t &gt;</class>
    <class kind="struct">movetk_core::is_string</class>
    <class kind="struct">movetk_core::is_string&lt; std::string &gt;</class>
    <class kind="struct">movetk_core::is_tuple</class>
    <class kind="struct">movetk_core::is_tuple&lt; const std::tuple&lt; Types... &gt; &gt;</class>
    <class kind="struct">movetk_core::is_tuple&lt; std::tuple&lt; Types... &gt; &gt;</class>
    <class kind="struct">movetk_core::is_Wrapper_Point</class>
    <class kind="struct">movetk_core::is_Wrapper_Point&lt; Kernel, const typename Kernel::Wrapper_Point &amp; &gt;</class>
    <class kind="struct">movetk_core::is_Wrapper_Point&lt; Kernel, typename Kernel::Wrapper_Point &gt;</class>
    <class kind="struct">movetk_core::MakeLine</class>
    <class kind="struct">movetk_core::MakeMinSphere</class>
    <class kind="struct">movetk_core::MakePoint</class>
    <class kind="struct">movetk_core::MakePolygon</class>
    <class kind="struct">movetk_core::MakeSegment</class>
    <class kind="struct">movetk_core::MakeSphere</class>
    <class kind="class">movetk_core::MBR</class>
    <class kind="struct">movetk_core::mbr_selector</class>
    <class kind="struct">movetk_core::mbr_selector&lt; GeometryTraits, Norm, void &gt;</class>
    <class kind="class">movetk_core::movetk_back_insert_iterator</class>
    <class kind="class">movetk_core::movetk_basic_iterator</class>
    <class kind="class">movetk_core::movetk_grid_iterator</class>
    <class kind="class">movetk_core::movetk_insert_iterator</class>
    <class kind="struct">movetk_core::MovetkGeometryKernel</class>
    <class kind="struct">movetk_core::Scaling</class>
    <class kind="class">movetk_core::SegmentIdGenerator</class>
    <class kind="struct">movetk_core::squared_distance_algorithm</class>
    <class kind="struct">movetk_core::squared_distance_algorithm&lt; GeometryTraits, Norm, void &gt;</class>
    <class kind="struct">movetk_core::Translation</class>
    <class kind="class">movetk_core::Wedge</class>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_size_t&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_size_t</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a5192129fe513a04df1806227c5f925b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_date&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_date_t</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a86945ffb9bd9cadc53b5bc4217209bce</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_NT&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_NT</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a314e8a963a2e98758edb872b67d1fd1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_random_access_iterator&lt; typename std::iterator_traits&lt; T &gt;::iterator_category &gt;::value, T &gt;::type</type>
      <name>requires_random_access_iterator</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a228aeece4617967d07f415e1afb0e12b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_output_iterator&lt; typename std::iterator_traits&lt; T &gt;::iterator_category &gt;::value, T &gt;::type</type>
      <name>requires_output_iterator</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a29421896067c68d715944445897b221b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkPoint&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_movetk_point</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a3419c3fc687241dade3c82db464c44e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkSegment&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_movetk_segment</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>aaab915fb8b6ff511427bff2acca7695d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkLine&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_movetk_Line</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>ab0b4a19a63d12a6a2f21a760f15f2d8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_Wrapper_Point&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_wrapper_point</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a65016293df2e312c04fb70c205b76da5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_pair&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_pair</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a3a66cac37ab51fd7df229337566c5f1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_equal_to&lt; T1, T2 &gt;::value, T2 &gt;::type</type>
      <name>requires_equality</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a509e9d52c86339907ba5bbc1c4001700</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_NT&lt; Traits, T &gt;::value||movetk_core::is_size_t&lt; T &gt;::value||movetk_core::is_date&lt; ParseDate &gt;::value||movetk_core::is_MovetkPoint&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_valid_type</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>aac05f5bc2d9b45fb47f9184736626cbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_string&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_string</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a32472f7f6df250cc350b7b6849adccc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; std::is_arithmetic&lt; T &gt;::value, T &gt;::type</type>
      <name>requires_arithmetic</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a0b467e229a160251d09a861898cfda5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>typename std::enable_if&lt; movetk_core::is_MovetkPoint&lt; Traits, T &gt;::value||movetk_core::is_MovetkLine&lt; Traits, T &gt;::value||movetk_core::is_MovetkSegment&lt; Traits, T &gt;::value, T &gt;::type</type>
      <name>requires_valid_type_distance_computation</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a7dd9abd1739b1fb3f18c712402346e72</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>NT</type>
      <name>deg2radians</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a3ba0ca287e0278fd3d425d381b5d7670</anchor>
      <arglist>(const NT degrees)</arglist>
    </member>
    <member kind="function">
      <type>NT</type>
      <name>rad2deg</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a3bfd571022fc97cdc9bc7af4f6983ce2</anchor>
      <arglist>(const NT radians)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is_sequence</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>af1f92171c2caf7ec3d88fcbdee331f7e</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>InputIterator</type>
      <name>min_non_zero_element</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>ad7788c6faac2e4e49de71689ddbeb0c8</anchor>
      <arglist>(InputIterator first, InputIterator beyond)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_time_diffs</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a9947fcacd7b9df537389e90b553141d0</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_distances</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a1f250e1e7a7b1f0cd03ca68c144f2270</anchor>
      <arglist>(InputIterator first, InputIterator beyond, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_speeds</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>abfb27ac2474ab425d277191cda17d453</anchor>
      <arglist>(TDiffIterator TdiffFirst, TDiffIterator TdiffBeyond, DistanceIterator DistFirst, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_headings</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a4ba2ccd9fd0981543c81b85382b18e38</anchor>
      <arglist>(InputIterator first, InputIterator beyond, PointsIterator pfirst, OutputIterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_velocities</name>
      <anchorfile>namespacemovetk__core.html</anchorfile>
      <anchor>a561fe757d7de59d12a4ec8180940aa4d</anchor>
      <arglist>(MagnitudeIterator Mfirst, MagnitudeIterator Mbeyond, DirectionIterator DFirst, OutputIterator iter)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>movetk_support</name>
    <filename>namespacemovetk__support.html</filename>
    <class kind="class">movetk_support::BinaryNode</class>
    <class kind="struct">movetk_support::BoostGeometryTraits</class>
    <class kind="struct">movetk_support::cast</class>
    <class kind="struct">movetk_support::cast&lt; long double &gt;</class>
    <class kind="struct">movetk_support::CGALTraits</class>
    <class kind="class">movetk_support::Discrete_Frechet</class>
    <class kind="class">movetk_support::Discrete_Hausdorff</class>
    <class kind="class">movetk_support::FiniteNorm</class>
    <class kind="class">movetk_support::FreeSpaceCell</class>
    <class kind="struct">movetk_support::FreeSpaceCellTraits</class>
    <class kind="class">movetk_support::FreeSpaceDiagram</class>
    <class kind="struct">movetk_support::FreeSpaceDiagramTraits</class>
    <class kind="class">movetk_support::InfinityNorm</class>
    <class kind="struct">movetk_support::is_valid_NT</class>
    <class kind="struct">movetk_support::is_valid_NT&lt; CGAL::Gmpfr &gt;</class>
    <class kind="struct">movetk_support::is_valid_NT&lt; CGAL::Gmpq &gt;</class>
    <class kind="struct">movetk_support::is_valid_NT&lt; CGAL::MP_Float &gt;</class>
    <class kind="struct">movetk_support::is_valid_NT&lt; CGAL::Mpzf &gt;</class>
    <class kind="struct">movetk_support::is_valid_NT&lt; long double &gt;</class>
    <class kind="struct">movetk_support::OutputRep</class>
    <class kind="struct">movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Point &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Polygon &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Sphere &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; false, Kernel, typename Kernel::Wrapper_Vector &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Point &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Polygon &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Sphere &gt;</class>
    <class kind="struct">movetk_support::OutputRep&lt; true, Kernel, typename Kernel::Wrapper_Vector &gt;</class>
    <class kind="struct">movetk_support::squared_distance_d</class>
    <class kind="struct">movetk_support::StackTraits</class>
    <class kind="class">movetk_support::StartStopDiagram</class>
    <class kind="class">movetk_support::StartStopDiagram&lt; SsdType::compressed, GeometryTraits, NodeType &gt;</class>
    <class kind="class">movetk_support::Tree</class>
    <class kind="class">movetk_support::TrieNode</class>
    <class kind="struct">movetk_support::TrieTraits</class>
    <class kind="struct">movetk_support::Wrapper_Boost_Discrete_Frechet_Distance</class>
    <class kind="struct">movetk_support::Wrapper_Boost_Discrete_Hausdorff_Distance</class>
    <class kind="struct">movetk_support::Wrapper_Boost_Kernel</class>
    <class kind="class">movetk_support::Wrapper_Boost_Line</class>
    <class kind="class">movetk_support::Wrapper_Boost_Point</class>
    <class kind="class">movetk_support::Wrapper_Boost_Polygon</class>
    <class kind="class">movetk_support::Wrapper_Boost_Segment</class>
    <class kind="class">movetk_support::Wrapper_Boost_Sphere</class>
    <class kind="struct">movetk_support::Wrapper_Boost_Squared_Distance</class>
    <class kind="class">movetk_support::Wrapper_Boost_Vector</class>
    <class kind="struct">movetk_support::Wrapper_CGAL_Kernel</class>
    <class kind="class">movetk_support::Wrapper_CGAL_Line</class>
    <class kind="class">movetk_support::Wrapper_CGAL_Point</class>
    <class kind="class">movetk_support::Wrapper_CGAL_Polygon</class>
    <class kind="class">movetk_support::Wrapper_CGAL_Segment</class>
    <class kind="class">movetk_support::Wrapper_CGAL_Sphere</class>
    <class kind="class">movetk_support::Wrapper_CGAL_Vector</class>
    <class kind="class">movetk_support::Wrapper_SEB</class>
    <member kind="function">
      <type>char</type>
      <name>f</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a75f9b88e76f5b6cf92fa3a53d221d1e0</anchor>
      <arglist>(char &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>const char</type>
      <name>f</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a57d547ec1ca58852160c9d507db8c9b7</anchor>
      <arglist>(const char &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>f</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a5acb93f515a01bd69661b5c0dcf0bc9a</anchor>
      <arglist>(int &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a20e4fcea109b9dc5cd44a9a7729300ad</anchor>
      <arglist>(std::ostream &amp;out, Wrapper_Boost_Polygon&lt; Kernel &gt; &amp;poly)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>ac495426a64bc843c8088c3486e5cea11</anchor>
      <arglist>(std::ostream &amp;out, const Wrapper_CGAL_Point&lt; Kernel &gt; &amp;point)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a83e02b662294a341ac4ddbfb74563771</anchor>
      <arglist>(std::ostream &amp;out, const Wrapper_CGAL_Vector&lt; Kernel &gt; &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a3173b2f6520eed013a7d800b10ed7caa</anchor>
      <arglist>(std::ostream &amp;out, Wrapper_CGAL_Polygon&lt; Kernel &gt; &amp;poly)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>join</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>a254e533bd19de6a2448c8deda42829d2</anchor>
      <arglist>(IteratorType first, IteratorType beyond, const char &amp;delim=&apos;,&apos;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>split</name>
      <anchorfile>namespacemovetk__support.html</anchorfile>
      <anchor>ac8100ee56459eb96ff1e73ae3485adf8</anchor>
      <arglist>(std::string &amp;in, OutputIterator iter, const char &amp;delim=&apos;,&apos;)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>GeomBackendDesign</name>
    <title>On the design of a geometry backend agnostic library</title>
    <filename>_geom_backend_design</filename>
    <docanchor file="_geom_backend_design" title="Design Goals">sec</docanchor>
    <docanchor file="_geom_backend_design" title="Design Patterns">subsection1</docanchor>
    <docanchor file="_geom_backend_design" title="The Interface">subsection2</docanchor>
    <docanchor file="_geom_backend_design" title="The CGAL Adapter">subsection3</docanchor>
  </compound>
  <compound kind="page">
    <name>md__Users_amitra_Documents_movetk_src_include_third_party_miniball_README</name>
    <title>README</title>
    <filename>md___users_amitra__documents_movetk_src_include_third_party_miniball__r_e_a_d_m_e</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>The Movement Toolkit</title>
    <filename>index</filename>
  </compound>
</tagfile>
