
add_executable (listen
	listen.cpp
)

add_executable (send
	send.cpp
)

add_executable (mockblock
	mockblock.cpp
)

add_executable (blockinfo
	blockinfo.cpp
)

add_executable (killall
	killall.cpp
)

add_executable (results
	results.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/machine
    ${Skweltch_SOURCE_DIR}/msg
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (listen
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (send
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (mockblock
	configLib
	coreLib
	machineLib
	boost_program_options
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (blockinfo
	configLib
	coreLib
	machineLib
	boost_program_options
	boost_filesystem
	boost_system
	log4cxx)

target_link_libraries (killall
	configLib
	coreLib
	boost_program_options
	boost_filesystem
	boost_system
	log4cxx)

target_link_libraries (results
	configLib
	coreLib
	boost_filesystem
	boost_system
	boost_regex
	boost_thread
	zmq
	czmq
	msgpack
	log4cxx
	mongoclient)

install(TARGETS listen send mockblock blockinfo killall results
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
