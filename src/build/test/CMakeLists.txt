
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (buildTest
	buildtest.cpp
)

target_link_libraries (buildTest 
	coreLib
	configLib
	buildLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(buildTest buildTest)
