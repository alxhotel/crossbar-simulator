cmake_minimum_required(VERSION 3.5)
project(crossbar_simulator)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")

# Make this a GUI application on Windows
if(WIN32)
	set(CMAKE_WIN32_EXECUTABLE ON)
endif()

# Libraries
include_directories(${CMAKE_SOURCE_DIR}/libs/libqasm/qasm_flex_bison/library)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/libqasm/qasm_flex_bison/library)

include_directories(${CMAKE_SOURCE_DIR}/libs/naxos/core)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/naxos/core)

include_directories(${CMAKE_SOURCE_DIR}/libs/json/include)
#add_subdirectory(${CMAKE_SOURCE_DIR}/libs/json)

include_directories(${CMAKE_SOURCE_DIR}/libs/intervals)
#add_subdirectory(${CMAKE_SOURCE_DIR}/libs/intervals)

# Source files
add_subdirectory(
	src bin
)
