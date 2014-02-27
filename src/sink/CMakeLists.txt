
add_executable (sinkavg
	sinkavg.cpp
)

add_executable (sinkcharfreq
	sinkcharfreq.cpp
)

add_executable (sinkelapsed
	SinkElapsed.cpp
)

add_executable (sinkstrings
	sinkstrings.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (sinkavg
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (sinkcharfreq
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (sinkelapsed
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (sinkstrings
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test) 
