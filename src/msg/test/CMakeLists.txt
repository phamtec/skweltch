
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (ignoreTest
	ignoretest.cpp
)

add_executable (msgTest
	msgtest.cpp
)

target_link_libraries (ignoreTest 
	coreLib
	configLib
	msgLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)

target_link_libraries (msgTest 
	coreLib
	configLib
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(pollTest ignoreTest)
add_test(portsTest msgTest)
