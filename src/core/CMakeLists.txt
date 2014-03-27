
add_library (coreLib SHARED
	Elapsed.cpp
	ExeRunner.cpp
	MsgTracker.cpp
	Poll.cpp
	Ports.cpp
	Sink.cpp
	Vent.cpp
	Work.cpp
	Poller.cpp
    Logging.cpp
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
	log4cxx
    boost_filesystem
    boost_system)

add_subdirectory (test)

install(TARGETS coreLib
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
