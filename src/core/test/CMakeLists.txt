
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (pollTest
	polltest.cpp
)

add_executable (portsTest
	portstest.cpp
)

add_executable (sinkTest
	sinktest.cpp
)

add_executable (trackTest
	tracktest.cpp
)

add_executable (ventTest
	venttest.cpp
)

add_executable (workTest
	worktest.cpp
)

target_link_libraries (pollTest 
	coreLib
	configLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)

target_link_libraries (portsTest 
	coreLib
	configLib
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (sinkTest 
	coreLib
	configLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (trackTest 
	coreLib
	configLib
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (ventTest 
	coreLib
	configLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (workTest 
	coreLib
	configLib
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(pollTest pollTest)
add_test(portsTest portsTest)
add_test(sinkTest sinkTest)
add_test(trackTest trackTest)
add_test(ventTest ventTest)
add_test(workTest workTest)
