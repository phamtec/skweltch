
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (tunerTest
	tunertest.cpp
)

target_link_libraries (tunerTest 
	coreLib
	configLib
	machineLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(tunerTest tunerTest)
