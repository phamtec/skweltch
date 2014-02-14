cmake_minimum_required (VERSION 2.8.12)
project (configTest)

enable_testing()

add_library (configLib STATIC IMPORTED)

set_target_properties (configLib PROPERTIES IMPORTED_LOCATION ../../dist/libconfig.dylib)

include_directories (
	../../json_spirit_v4.06/json_spirit
	../../turtle)

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
	configLib
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (jsonTest 
	log4cxx
	configLib
	boost_unit_test_framework
	boost_test_exec_monitor)
	
target_link_libraries (pathTest 
	log4cxx
	configLib
	boost_unit_test_framework
	boost_test_exec_monitor)
	
add_test(mainTest mainTest)
add_test(jsonTest jsonTest)
add_test(pathTest pathTest)
