
include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
	${Skweltch_SOURCE_DIR}/turtle)

add_executable (mainTest
	configtest.cpp
)

add_executable (jsonTest
	jsontest.cpp
)

add_executable (pathTest
	pathtest.cpp
)

target_link_libraries (mainTest 
	log4cxx
	config
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (jsonTest 
	log4cxx
	config
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (pathTest 
	log4cxx
	config
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(mainTest mainTest)
add_test(jsonTest jsonTest)
add_test(pathTest pathTest)
