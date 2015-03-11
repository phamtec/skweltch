cmake_minimum_required (VERSION 2.6)

project (Skweltch)

################################
# Boost
################################
add_definitions( -DBOOST_LOG_DYN_LINK )
add_definitions( -DBoost_USE_STATIC_LIBS=ON )
set_property( GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON )
set( Boost_INCLUDE_DIR /opt/boost_1_57_0 )
set( Boost_LIBRARY_DIR /opt/boost_1_57_0/stage/lib )
find_package( Boost 1.57.0 REQUIRED thread system log log_setup program_options )
find_package( Threads )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIR} )

include_directories( /usr/local/include )
link_directories( /usr/local/lib )

enable_testing()

add_subdirectory (config) 
add_subdirectory (core) 
add_subdirectory (msg) 
add_subdirectory (machine) 
add_subdirectory (graph) 
add_subdirectory (pipe) 
add_subdirectory (sink) 
add_subdirectory (util) 
add_subdirectory (vent) 
add_subdirectory (work) 
add_subdirectory (build) 
add_subdirectory (pugixml/scripts) 

include (CTest)
