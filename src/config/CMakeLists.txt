cmake_minimum_required (VERSION 2.8.12)
project (config)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ../dist)

add_library (config SHARED
	JsonArray.cpp
	JsonNamePredicate.cpp
	JsonObject.cpp
	JsonPath.cpp
)

include_directories (
	../json_spirit_v4.06/json_spirit)

target_link_libraries (config 
	log4cxx)
