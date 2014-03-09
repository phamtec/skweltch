
add_library (ventLib SHARED
	WordSplitter.cpp
	XmlParser.cpp
	XPathSplitter.cpp
)

add_executable (ventrandomnum
	VentRandomNum.cpp
)

add_executable (ventwords
	ventwords.cpp
)

add_executable (ventxml
	ventxml.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/msg
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (ventLib
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (ventrandomnum
	configLib
	coreLib
    boost_program_options
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (ventwords
	ventLib
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (ventxml
	ventLib
	configLib
	coreLib
	boost_filesystem
	boost_system
	boost_iostreams
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test) 

install(TARGETS ventLib ventrandomnum ventwords ventxml
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
