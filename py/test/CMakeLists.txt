# Use Python unittest(?) for creating unittests on the bindings.

find_package(Python3 COMPONENTS Interpreter 3.7.1)

add_custom_target(PyMoveTk_test
	COMMENT "Running PyMoveTk tests with Python executable: ${Python3_EXECUTABLE}"
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=$<TARGET_FILE_DIR:PyMoveTk>;$ENV{PYTHONPATH}" ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/PyMoveTk_test.py
)