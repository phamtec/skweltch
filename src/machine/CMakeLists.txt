
add_library (machineLib SHARED
	MachineRunner.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/monitor
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/msg
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (machineLib
	configLib
	monitorLib
	zmq
	czmq
	msgpack
	boost_filesystem
	boost_system
	log4cxx)

add_executable (machine
	machine.cpp)

target_link_libraries (machine
	configLib
	coreLib
	monitorLib
	machineLib
	boost_program_options
	zmq
	czmq
	msgpack
	log4cxx)

