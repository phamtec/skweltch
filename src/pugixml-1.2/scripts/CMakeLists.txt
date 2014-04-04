project(pugixml)

cmake_minimum_required(VERSION 2.6)
set(BUILD_SHARED_LIBS ON)

set(HEADERS ../src/pugixml.hpp ../src/pugiconfig.hpp)
set(SOURCES ${HEADERS} ../src/pugixml.cpp)

if(BUILD_SHARED_LIBS)
    add_library(pugixml SHARED ${SOURCES})
else()
    add_library(pugixml STATIC ${SOURCES})
endif()

set_target_properties(pugixml PROPERTIES VERSION 1.2 SOVERSION 1.2)

install(TARGETS pugixml 
			LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin 
			ARCHIVE DESTINATION ${Skweltch_SOURCE_DIR}/bin)
