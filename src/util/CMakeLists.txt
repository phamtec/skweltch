
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

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/machine
    ${Skweltch_SOURCE_DIR}/monitor
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
	monitorLib
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
	monitorLib
	boost_program_options
	boost_filesystem
	boost_system
	log4cxx)

install(TARGETS listen send mockblock blockinfo
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)