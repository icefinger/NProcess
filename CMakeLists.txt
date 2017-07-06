# (1)
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(NProcess)


# check for x64 or x86
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if (${LIB64} STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

#keeping root elements for possible tests
#include(FindROOT.cmake)
#root for tests


#link_directories(${ROOT_LIBRARY_DIR})
#link_directories($ENV{LIBCONFIG_ROOT/lib})

#include_directories(${ROOT_INCLUDE_DIR})



# (4) organize project libraries
file(GLOB sources ./src/[a-zA-Z0-9]*.cc)
file(GLOB headers ./include/*)

foreach(adir IN LISTS sources)
  if (adir MATCHES .*/tests/+src.*)
    list (REMOVE_ITEM sources ${adir})
  endif ()
endforeach ()

foreach(adir IN LISTS headers)
  if (adir MATCHES .*/tests/+include/)
    list (REMOVE_ITEM headers ${adir})
  endif ()
endforeach ()


file(GLOB test_sources tests/src/[a-zA-Z0-9]*.cc)
file(GLOB test_headers tests/include/)

#list(REMOVE_ITEM sources ${test_sources})

#message (${sources})
#message (${test_sources})

file(GLOB PROGRAMS programs/[a-zA-Z1-9]*.cxx)
file(GLOB TESTS tests/[a-zA-Z1-9]*.cxx)
include_directories (${headers})

# (5) set the needed cflags
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -W -Wall -pedantic -Wno-non-virtual-dtor -Wno-long-long -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-variadic-macros -Wshadow -pipe -std=c++11 -DPROJECT_SOURCE_DIR=\\\"${PROJECT_SOURCE_DIR}\\\"")

#add other
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")


if (DEBUG)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else ()
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()

if (FULL_OPTIMIZATION)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -flto")
endif (FULL_OPTIMIZATION)

if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.8.9")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
endif(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.8.9")


SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__CXX_MIN_VERSION=${CMAKE_CXX_COMPILER_VERSION}")


message (${CMAKE_CXX_FLAGS})


# (6) finish with all the binaries
if(ENABLE_TESTING)
  add_library(NProcess SHARED ${sources} ${test_sources})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_TESTING=1")
  foreach(aprog IN LISTS TESTS)
    get_filename_component(aprogname ${aprog} NAME_WE)
    add_executable(${aprogname} ${aprog} ${headers} ${test_headers})
    target_link_libraries(${aprogname} ${ROOT_LIBRARIES} NProcess)
  endforeach ()
else(ENABLE_TESTING)
  add_library(NProcess SHARED ${sources})
  foreach(aprog IN LISTS PROGRAMS)
    get_filename_component(aprogname ${aprog} NAME_WE)
    add_executable(${aprogname} ${aprog} ${headers})
    target_link_libraries(${aprogname} ${ROOT_LIBRARIES} NProcess)
  endforeach ()
endif(ENABLE_TESTING)
