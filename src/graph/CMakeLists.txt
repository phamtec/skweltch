
add_library (graphLib SHARED
	MachineGraph.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/msg
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (graphLib
	configLib
	msgLib
	coreLib
	zmq
	czmq
	msgpack
	boost_filesystem
	boost_system
	log4cxx)

add_executable (graph
	graph.cpp)

target_link_libraries (graph
	graphLib
	boost_program_options
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test) 

install(TARGETS graphLib graph
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
