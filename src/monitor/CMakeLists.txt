
add_library (monitorLib SHARED
	PipeBuilder.cpp
	TaskMonitor.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (monitorLib
	coreLib
	configLib
	msgLib
	zmq
	czmq
	msgpack
	log4cxx)

add_executable (monitor
	monitor.cpp)

target_link_libraries (monitor
	coreLib
	configLib
	monitorLib
	boost_program_options
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test)
