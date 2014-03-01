
add_library (configLib SHARED
	JsonArray.cpp
	JsonNamePredicate.cpp
	JsonObject.cpp
	JsonPath.cpp
)

include_directories (
	${Skweltch_SOURCE_DIR}/json_spirit_v4.06/json_spirit)

target_link_libraries (configLib 
	log4cxx)

add_subdirectory (test)

install(TARGETS configLib
            LIBRARY DESTINATION ${Skweltch_SOURCE_DIR}/bin)
