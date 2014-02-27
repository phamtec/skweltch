
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (freqTest
	freqtest.cpp
)

add_executable (xpath2Test
	xpathtest.cpp
)

target_link_libraries (freqTest 
	coreLib
	configLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (xpath2Test 
	coreLib
	configLib
	pugiXML
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(freqTest freqTest)
add_test(xpath2Test xpath2Test)
