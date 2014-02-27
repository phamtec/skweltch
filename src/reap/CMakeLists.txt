
add_executable (grimreaper
	GrimReaper.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (grimreaper
	configLib
	coreLib
	zmq
	czmq
	msgpack
	log4cxx)
