
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (wordsTest
	wordstest.cpp
)

add_executable (xpathTest
	xpathtest.cpp
)

target_link_libraries (wordsTest 
	coreLib
	configLib
	ventLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (xpathTest 
	ventLib
	coreLib
	configLib
	zmq
	log4cxx
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(wordsTest wordsTest)
add_test(xpathTest xpathTest)
