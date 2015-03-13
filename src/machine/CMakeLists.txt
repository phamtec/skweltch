
add_library (machineLib SHARED
	MachineRunner.cpp 
	Results.cpp 
	MachineTuner.cpp
	PipeBuilder.cpp
	TaskMonitor.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/msg
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (machineLib
	configLib
	coreLib
	zmq
	czmq
	msgpack
	boost_filesystem
	boost_system
	log4cxx)

add_executable (machine
	machine.cpp)

add_executable (machinecmd
	machinecmd.cpp)

target_link_libraries (machine
	configLib
	coreLib
	machineLib
	boost_program_options
	boost_regex
	boost_thread
	zmq
	czmq
	msgpack
	log4cxx
	mongoclient)

target_link_libraries (machinecmd
	configLib
	coreLib
	machineLib
	boost_program_options
	zmq
	czmq
	msgpack
	log4cxx)

add_executable (soak
	soak.cpp
)

add_executable (tune
	tune.cpp
)

add_executable (run
	run.cpp
)

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
	configLib
	coreLib
	machineLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (run
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

install(TARGETS machineLib machine machinecmd soak tune run
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
