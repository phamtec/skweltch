
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (graphTest
	graphtest.cpp
)

target_link_libraries (graphTest 
	graphLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(graphTest graphTest)
