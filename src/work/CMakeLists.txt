
add_executable (workcharcount
	workcharcount.cpp
)

add_executable (workcharfreq
	workcharfreq.cpp
)

add_executable (worksleep
	WorkSleep.cpp
)

add_executable (workxpath
	workxpath.cpp
)

add_library (pugiXML STATIC
	${Skweltch_SOURCE_DIR}/pugixml/src/pugixml.cpp
)

include_directories (
    ${Skweltch_SOURCE_DIR}/config
    ${Skweltch_SOURCE_DIR}/core
    ${Skweltch_SOURCE_DIR}/msg
    ${Skweltch_SOURCE_DIR}/pugixml/src
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit
    ${Skweltch_SOURCE_DIR}/cppzmq-master
	)

target_link_libraries (workcharcount
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (workcharfreq
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (worksleep
	configLib
	coreLib
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

target_link_libraries (workxpath
	configLib
	coreLib
	pugiXML
	boost_filesystem
	boost_system
	zmq
	czmq
	msgpack
	log4cxx)

add_subdirectory (test) 

install(TARGETS workcharcount workcharfreq worksleep workxpath
			RUNTIME DESTINATION ${Skweltch_SOURCE_DIR}/bin
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
