
add_library (msgLib SHARED
	IgnoreWhileWorking.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (msgLib
	configLib
	coreLib
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test)
	
install(TARGETS msgLib
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
