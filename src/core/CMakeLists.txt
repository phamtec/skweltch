
add_library (coreLib SHARED
	Elapsed.cpp
	ExeRunner.cpp
	MsgTracker.cpp
	Poll.cpp
	Ports.cpp
	Sink.cpp
	Vent.cpp
	Work.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (coreLib
	configLib
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test)
