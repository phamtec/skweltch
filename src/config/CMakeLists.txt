
add_library (config SHARED
	JsonArray.cpp
	JsonNamePredicate.cpp
	JsonObject.cpp
	JsonPath.cpp
)

include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit)

target_link_libraries (config 
	log4cxx)

add_subdirectory (test)
