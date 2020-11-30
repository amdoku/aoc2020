cmake_minimum_required(VERSION 3.2)
project(aos2020_sample
	VERSION 1.0.0
	LANGUAGES CXX)

# compile_commands.json for clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ std
set(CMAKE_CXX_STANDARD 17)

# Force CMAKE_BUILD_TYPE to be set
if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "Defaulting to build type 'RelWithDebInfo'")
	set (CMAKE_BUILD_TYPE
		RelWithDebInfo
		CACHE STRING "Build Type." FORCE)
	set_property(
		CACHE CMAKE_BUILD_TYPE
		PROPERTY STRINGS
			"Debug"
			"Release"
			"MinSizeRel"
			"RelWithDebInfo")
endif()

# Set warnings
if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Werror -Wextra)
endif()

# binary
add_executable(solution main.cpp)

# lib (for demo purposes)
add_library(Value 
	value.hpp
	value.cpp
	)

# Link binary with lib to get executable
target_link_libraries(solution PRIVATE Value)

