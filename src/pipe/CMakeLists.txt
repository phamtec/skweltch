
add_executable (pipecount
	pipecount.cpp
)

add_executable (pipedump
	pipedump.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (pipecount
	configLib
	coreLib
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (pipedump
	configLib
	coreLib
	zmq
	czmq
	msgpack
	log4cxx)
	
install(TARGETS pipecount pipedump
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
