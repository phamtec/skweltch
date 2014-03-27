
add_library (buildLib SHARED
	FileModChecker.cpp
	MakeAnalyser.cpp
)

add_executable (polldir
	polldir.cpp
)

add_executable (sinkbuild
	sinkbuild.cpp
)

add_executable (workbash
	workbash.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/msg
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (buildLib
	configLib
	coreLib
	boost_filesystem
	boost_system
	boost_regex
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (polldir
	configLib
	coreLib
	buildLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (sinkbuild
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)
target_link_libraries (workbash
	configLib
	coreLib
	buildLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test) 

install(TARGETS buildLib polldir sinkbuild workbash
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
