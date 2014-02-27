
add_library (tuneLib SHARED
	MachineTuner.cpp
)

add_executable (soak
	soak.cpp
)

add_executable (tune
	tune.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/machine
    ${Skweltch_SOURCE_DIR}/monitor
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (tuneLib
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (soak
	configLib
	coreLib
	machineLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (tune
	tuneLib
	configLib
	coreLib
	machineLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test) 
